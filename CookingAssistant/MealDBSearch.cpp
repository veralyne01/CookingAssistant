#define NOMINMAX   // иначе windows.h определяет макросы min/max и ломает std::min/std::max
#include <windows.h>
#include <winhttp.h>
#pragma comment(lib, "winhttp.lib")

#include "mealdbsearch.h"
#include <nlohmann/json.hpp>
#include <cctype>
#include <cstdio>
#include "user.h"
#include <unordered_set>
#include "breakfastpicker.h"
#include "FoodDataCentral.h"

using json = nlohmann::json;

namespace {

    // Закрывает дескриптор WinHTTP сам, даже при раннем return
    struct HttpHandle {
        HINTERNET h;
        explicit HttpHandle(HINTERNET handle) : h(handle) {}
        ~HttpHandle() { if (h) WinHttpCloseHandle(h); }
        HttpHandle(const HttpHandle&) = delete;
        HttpHandle& operator=(const HttpHandle&) = delete;
    };

    // Кодирует строку для URL (проценты по байтам UTF-8)
    std::string url_encode(const std::string& s) {
        std::string out;
        char buf[4];
        for (unsigned char c : s) {
            if (std::isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
                out += static_cast<char>(c);
            }
            else {
                std::snprintf(buf, sizeof(buf), "%%%02X", c);
                out += buf;
            }
        }
        return out;
    }

    // GET-запрос. Возвращает тело ответа или пустую строку при любой ошибке.
    std::string http_get(const std::string& path) {
        const std::wstring wpath(path.begin(), path.end()); // path уже закодирован: только ASCII

        HttpHandle session(WinHttpOpen(L"CookingAssistant/1.0",
            WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0));
        if (!session.h) return {};
        WinHttpSetTimeouts(session.h, 5000, 5000, 10000, 10000); // не висим вечно без интернета

        HttpHandle connection(WinHttpConnect(session.h, L"www.themealdb.com",
            INTERNET_DEFAULT_HTTPS_PORT, 0));
        if (!connection.h) return {};

        HttpHandle request(WinHttpOpenRequest(connection.h, L"GET", wpath.c_str(), nullptr,
            WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_SECURE));
        if (!request.h) return {};

        if (!WinHttpSendRequest(request.h, WINHTTP_NO_ADDITIONAL_HEADERS, 0,
            WINHTTP_NO_REQUEST_DATA, 0, 0, 0) ||
            !WinHttpReceiveResponse(request.h, nullptr)) {
            return {};
        }

        DWORD status = 0;
        DWORD statusSize = sizeof(status);
        if (!WinHttpQueryHeaders(request.h, WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
            WINHTTP_HEADER_NAME_BY_INDEX, &status, &statusSize, WINHTTP_NO_HEADER_INDEX) ||
            status != 200) {
            return {};
        }

        std::string body;
        DWORD available = 0;
        while (WinHttpQueryDataAvailable(request.h, &available) && available > 0) {
            std::string chunk(available, '\0');
            DWORD got = 0;
            if (!WinHttpReadData(request.h, chunk.data(), available, &got)) return {};
            body.append(chunk.data(), got);
        }
        return body;
    }

    // Значение строкового поля или "" (поле может отсутствовать или быть null)
    std::string text(const json& j, const std::string& key) {
        auto it = j.find(key);
        if (it != j.end() && it->is_string()) return it->get<std::string>();
        return {};
    }

    std::string trim(const std::string& s) {
        const char* ws = " \t\r\n";
        const auto b = s.find_first_not_of(ws);
        if (b == std::string::npos) return {};
        const auto e = s.find_last_not_of(ws);
        return s.substr(b, e - b + 1);
    }

    // Одна запись "meals" -> Dish
    Dish dish_from_json(const json& m) {
        Dish d;
        d.id = text(m, "idMeal");
        d.name = text(m, "strMeal");
        d.category = text(m, "strCategory");
        d.area = text(m, "strArea");
        if (d.area.empty()) d.area = text(m, "strCountry");
        d.instructions = text(m, "strInstructions");

        for (int i = 1; i <= 20; ++i) {
            const std::string ing =
                trim(text(m, "strIngredient" + std::to_string(i)));

            if (ing.empty()) continue;

            Ingredient ingredient;
            ingredient.name = ing;
            ingredient.measure =
                trim(text(m, "strMeasure" + std::to_string(i)));

            d.ingredients.push_back(ingredient);
        }

        return d;
    }

    // Достаёт массив "meals". Если блюд нет (meals: null), тело пустое или JSON битый, вернёт [].
    json meals_array(const std::string& body) {
        if (body.empty()) return json::array();
        try {
            const json root = json::parse(body);
            if (root.contains("meals") && root.at("meals").is_array()) return root.at("meals");
        }
        catch (const json::exception&) {
            // битый ответ считаем отсутствием данных
        }
        return json::array();
    }

} // namespace

namespace MealDB {

    std::vector<std::string> filter_ids_by_category(const std::string& category) {
        std::vector<std::string> ids;
        const json meals = meals_array(http_get("/api/json/v1/1/filter.php?c=" + url_encode(category)));
        for (const auto& m : meals) {
            const std::string id = text(m, "idMeal");
            if (!id.empty()) ids.push_back(id);
        }
        return ids;
    }

    std::vector<Dish> search_by_name(const std::string& query) {
        std::vector<Dish> dishes;
        const json meals = meals_array(http_get("/api/json/v1/1/search.php?s=" + url_encode(query)));
        for (const auto& m : meals) dishes.push_back(dish_from_json(m));
        return dishes;
    }

    std::optional<Dish> lookup_by_id(const std::string& id) {
        const json meals = meals_array(http_get("/api/json/v1/1/lookup.php?i=" + url_encode(id)));
        if (meals.empty()) return std::nullopt;
        return dish_from_json(meals.front());
    }


    void show_result(User& user) {
        static std::unordered_set<std::string> shown;

        auto dish = Breakfast::suggest(user, shown);

        if (!dish) {
            std::cout << "\nNo suitable breakfast found.\n";
            return;
        }

        std::cout << "\n========================================\n";
        std::cout << "             " << dish->name << "\n";
        std::cout << "========================================\n";

        if (!dish->category.empty())
            std::cout << "Category: " << dish->category << "\n";

        if (!dish->area.empty())
            std::cout << "Cuisine: " << dish->area << "\n";

        // INGREDIENTS
        std::cout << "\nINGREDIENTS:\n";

        for (size_t i = 0; i < dish->ingredients.size(); ++i) {
            std::cout << " - " << dish->ingredients[i].name;

            if (i < dish->ingredients.size() &&
                !dish->ingredients[i].measure.empty()) {
                std::cout << " — " << dish->ingredients[i].measure;
            }

            std::cout << "\n";
        }
        FoodDataCentral::calculate_dish(*dish);
        std::cout << "\nNUTRITION:\n";

        std::cout << "Calories: "
            << dish->total_nutrition.calories << " kcal\n";

        std::cout << "Proteins: "
            << dish->total_nutrition.proteins << " g\n";

        std::cout << "Fats: "
            << dish->total_nutrition.fats << " g\n";

        std::cout << "Carbohydrates: "
            << dish->total_nutrition.carbohydrates << " g\n";

        std::cout << "\nPer serving (" << dish->servings << "):\n";

        std::cout << "Calories: "
            << dish->nutrition_per_serving.calories << " kcal\n";

        std::cout << "Proteins: "
            << dish->nutrition_per_serving.proteins << " g\n";

        std::cout << "Fats: "
            << dish->nutrition_per_serving.fats << " g\n";

        std::cout << "Carbohydrates: "
            << dish->nutrition_per_serving.carbohydrates << " g\n";

        if (!dish->nutrition_complete) {
            std::cout << "\nWarning: nutrition data is incomplete. "
                << "Some ingredients could not be calculated.\n";
        }

        // INSTRUCTIONS
        std::cout << "\nINSTRUCTIONS:\n";

        if (!dish->instructions.empty()) {
            std::cout << dish->instructions << "\n";
        }
        else {
            std::cout << "Instructions are not available.\n";
        }

        std::cout << "\n========================================\n";
    }

    
}