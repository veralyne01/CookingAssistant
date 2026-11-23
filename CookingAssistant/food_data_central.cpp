#define NOMINMAX

#include <windows.h>
#include <winhttp.h>
#pragma comment(lib, "winhttp.lib")
#include <iostream>
#include <algorithm>
#include <cctype>
#include <vector>
#include "food_data_central.h"
#include <nlohmann/json.hpp>

#include <cstdlib>
#include <cstdio>
#include <string>
#include <optional>
#include "nutrition_calculator.h"
#include "measure_converter.h"

using json = nlohmann::json;

namespace {

    struct HttpHandle {
        HINTERNET h;

        explicit HttpHandle(HINTERNET handle) : h(handle) {}

        ~HttpHandle() {
            if (h) WinHttpCloseHandle(h);
        }

        HttpHandle(const HttpHandle&) = delete;
        HttpHandle& operator=(const HttpHandle&) = delete;
    };

    // Результат запроса: код ответа и тело.
    struct HttpResult {
        DWORD status = 0;   // 0: запрос вообще не удалось выполнить (нет сети, таймаут)
        std::string body;
    };

    std::string url_encode(const std::string& value) {
        std::string result;
        char buffer[4];

        for (unsigned char c : value) {
            if ((c >= 'a' && c <= 'z') ||
                (c >= 'A' && c <= 'Z') ||
                (c >= '0' && c <= '9') ||
                c == '-' || c == '_' || c == '.' || c == '~') {
                result += static_cast<char>(c);
            }
            else {
                std::snprintf(buffer, sizeof(buffer), "%%%02X", c);
                result += buffer;
            }
        }

        return result;
    }

    // BEGIN HELPERS

    // Убирает пробелы, переводы строк и кавычки по краям ключа.
    // Невидимый символ в ключе (например, '\r' или пробел) попадает прямо в строку запроса,
    // и nginx отвечает на такой запрос "400 Bad Request".
    std::string clean_key(const std::string& raw) {
        auto is_junk = [](unsigned char c) {
            return std::isspace(c) || c == '"' || c == '\'';
            };

        std::size_t begin = 0;
        std::size_t end = raw.size();

        while (begin < end && is_junk(static_cast<unsigned char>(raw[begin]))) ++begin;
        while (end > begin && is_junk(static_cast<unsigned char>(raw[end - 1]))) --end;

        return raw.substr(begin, end - begin);
    }

    // Ключ api.data.gov состоит только из латинских букв и цифр.
    bool key_looks_valid(const std::string& key) {
        if (key.empty()) return false;

        for (unsigned char c : key) {
            if (!std::isalnum(c)) return false;
        }
        return true;
    }

    // Адрес запроса поиска в том виде, как он показан в документации USDA.
    // restrict_types = true добавляет фильтр по типам данных (обычные продукты):
    // запятая без кодирования, пробел как %20, без скобок.
    std::string build_search_path(
        const std::string& key,
        const std::string& query,
        bool restrict_types
    ) {
        std::string path =
            "/fdc/v1/foods/search?api_key=" + key +
            "&query=" + url_encode(query) +
            "&pageSize=" + (restrict_types ? "5" : "10");

        if (restrict_types)
            path += "&dataType=SR%20Legacy,Foundation";

        return path;
    }

    // END HELPERS

    // Адрес состоит только из ASCII (всё лишнее уже закодировано),
    // поэтому простое расширение символов безопасно.
    std::wstring to_wide(const std::string& ascii) {
        return std::wstring(ascii.begin(), ascii.end());
    }

    HttpResult http_get(const std::wstring& path) {
        HttpResult result;

        HttpHandle session(WinHttpOpen(
            L"CookingAssistant/1.0",
            WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
            WINHTTP_NO_PROXY_NAME,
            WINHTTP_NO_PROXY_BYPASS,
            0
        ));

        if (!session.h) return result;

        WinHttpSetTimeouts(session.h, 5000, 5000, 10000, 10000);

        HttpHandle connection(WinHttpConnect(
            session.h,
            L"api.nal.usda.gov",
            INTERNET_DEFAULT_HTTPS_PORT,
            0
        ));

        if (!connection.h) return result;

        HttpHandle request(WinHttpOpenRequest(
            connection.h,
            L"GET",
            path.c_str(),
            nullptr,
            WINHTTP_NO_REFERER,
            WINHTTP_DEFAULT_ACCEPT_TYPES,
            WINHTTP_FLAG_SECURE
        ));

        if (!request.h) return result;

        if (!WinHttpSendRequest(
            request.h,
            WINHTTP_NO_ADDITIONAL_HEADERS,
            0,
            WINHTTP_NO_REQUEST_DATA,
            0,
            0,
            0
        )) return result;

        if (!WinHttpReceiveResponse(request.h, nullptr))
            return result;

        // 1. Получаем HTTP-статус.
        DWORD status = 0;
        DWORD status_size = sizeof(status);

        if (!WinHttpQueryHeaders(
            request.h,
            WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
            WINHTTP_HEADER_NAME_BY_INDEX,
            &status,
            &status_size,
            WINHTTP_NO_HEADER_INDEX
        )) return result;

        result.status = status;

        // 2. Читаем тело ответа независимо от статуса.
        DWORD available = 0;

        while (WinHttpQueryDataAvailable(request.h, &available) && available > 0) {
            std::string buffer(available, '\0');
            DWORD downloaded = 0;

            if (!WinHttpReadData(
                request.h,
                buffer.data(),
                available,
                &downloaded
            )) break;

            result.body.append(buffer.data(), downloaded);
        }

        // 3. Решение по статусу принимает вызывающий код.
        return result;
    }

    bool should_ignore_nutrition(
        const std::string& ingredient_name,
        const std::string& measure
    ) {
        // Приводим строку к нижнему регистру.
        auto normalize = [](std::string value) {
            std::transform(
                value.begin(),
                value.end(),
                value.begin(),
                [](unsigned char c) {
                    return static_cast<char>(std::tolower(c));
                }
            );

            return value;
            };

        const std::string name = normalize(ingredient_name);
        const std::string amount = normalize(measure);

        // Не учитываем неопределённые количества.
        if (amount.find("to taste") != std::string::npos ||
            amount.find("pinch") != std::string::npos ||
            amount.find("garnish") != std::string::npos) {
            return true;
        }

        // Список соли и специй, калорийность которых игнорируем.
        const std::vector<std::string> ignored_ingredients = {
            "salt",
            "sea salt",
            "table salt",
            "kosher salt",

            "pepper",
            "black pepper",
            "white pepper",
            "red pepper flakes",

            "allspice",
            "cinnamon",
            "paprika",
            "nutmeg",
            "cumin",
            "coriander",
            "turmeric",
            "ginger powder",
            "garlic powder",
            "onion powder",
            "cayenne",
            "cayenne pepper",
            "chili powder",
            "chilli powder",
            "chili flakes",
            "chilli flakes",
            "cloves",
            "seasoning",
            "old bay"
        };

        return std::find(
            ignored_ingredients.begin(),
            ignored_ingredients.end(),
            name
        ) != ignored_ingredients.end();
    }

    bool is_generic_food(const json& food) {
        auto it = food.find("dataType");

        if (it == food.end() || !it->is_string())
            return true;

        const std::string type = it->get<std::string>();

        return type == "Foundation" ||
            type == "SR Legacy" ||
            type == "Survey (FNDDS)";
    }

    std::optional<Nutrition> parse_nutrition(const json& food) {
        if (!food.contains("foodNutrients") ||
            !food["foodNutrients"].is_array()) {
            return std::nullopt;
        }

        Nutrition result{};

        bool has_calories = false;
        bool has_proteins = false;
        bool has_fats = false;
        bool has_carbs = false;

        for (const auto& nutrient : food["foodNutrients"]) {
            const std::string name =
                nutrient.value("nutrientName", "");

            std::string unit =
                nutrient.value("unitName", "");

            // Приводим единицы измерения к нижнему регистру.
            std::transform(
                unit.begin(),
                unit.end(),
                unit.begin(),
                [](unsigned char c) {
                    return static_cast<char>(std::tolower(c));
                }
            );

            if (!nutrient.contains("value")) continue;
            double value = 0.0;
            const auto& raw_value = nutrient["value"];
            if (raw_value.is_number()) {
                value = raw_value.get<double>();
            } else if (raw_value.is_string()) {
                std::string raw = raw_value.get<std::string>();
                const auto first = raw.find_first_not_of(" \t\r\n");
                if (first == std::string::npos) continue;
                const auto last = raw.find_last_not_of(" \t\r\n");
                raw = raw.substr(first, last - first + 1);
                std::replace(raw.begin(), raw.end(), ',', '.');
                try {
                    std::size_t parsed = 0;
                    value = std::stod(raw, &parsed);
                    if (parsed != raw.size()) continue;
                } catch (const std::exception&) {
                    continue;
                }
            } else {
                continue;
            }

            if (name == "Energy" &&
                (unit == "kcal" || unit == "kj")) {

                if (!has_calories || unit == "kcal") {
                    result.calories =
                        unit == "kj" ? value / 4.184 : value;
                    has_calories = true;
                }
            }
            else if (name == "Protein" && unit == "g") {
                result.proteins = value;
                has_proteins = true;
            }
            else if (name == "Total lipid (fat)" && unit == "g") {
                result.fats = value;
                has_fats = true;
            }
            else if (name == "Carbohydrate, by difference" &&
                unit == "g") {
                result.carbohydrates = value;
                has_carbs = true;
            }
        }

        if (!has_calories || !has_proteins ||
            !has_fats || !has_carbs) {
            return std::nullopt;
        }

        return result;
    }


}

namespace FoodDataCentral {


    std::optional<Nutrition> get_nutrition(
        const std::string& ingredient_name
    ) {
        // 1. Ключ из переменной окружения. Его нигде не печатаем.
        char* raw_key = nullptr;
        size_t raw_key_size = 0;

        if (_dupenv_s(&raw_key, &raw_key_size, "FDC_API_KEY") != 0 ||
            raw_key == nullptr) {
            std::cout << "[USDA] FDC_API_KEY is not set\n";
            return std::nullopt;
        }

        const std::string key = clean_key(raw_key);
        free(raw_key);

        if (!key_looks_valid(key)) {
            // Показываем только длину, а не сам ключ.
            std::cout << "[USDA] FDC_API_KEY looks wrong (length "
                << key.size()
                << "): it must contain only letters and digits\n";
            return std::nullopt;
        }

        std::cout << "[USDA] Searching: " << ingredient_name << '\n';

        // 2. Сначала запрос с фильтром по типам данных.
        // Если сервер отклонил его (400), повторяем запрос без фильтра:
        // так сразу видно, виноват ли фильтр.
        HttpResult response = http_get(
            to_wide(build_search_path(key, ingredient_name, true))
        );

        if (response.status == 400) {
            std::cout << "[USDA] HTTP 400 with dataType filter, "
                "retrying without it\n";

            response = http_get(
                to_wide(build_search_path(key, ingredient_name, false))
            );
        }

        if (response.status != 200) {
            if (response.status == 0) {
                std::cout << "[USDA] No response (network problem?)\n";
            }
            else if (response.status == 403) {
                std::cout << "[USDA] HTTP 403: the API key was rejected\n";
            }
            else if (response.status == 429) {
                std::cout << "[USDA] HTTP 429: rate limit exceeded, "
                    "try again later\n";
            }
            else {
                std::cout << "[USDA] HTTP error: " << response.status << '\n';
            }

            return std::nullopt;
        }


        try {
            const json root = json::parse(response.body);

            if (!root.contains("foods") ||
                !root["foods"].is_array() ||
                root["foods"].empty()) {

                std::cout << "[USDA] No products found: "
                    << ingredient_name << '\n';

                return std::nullopt;
            }

            for (const auto& food : root["foods"]) {
                if (!is_generic_food(food))
                    continue;

                auto nutrition = parse_nutrition(food);

                if (nutrition.has_value()) {
                    std::cout << "[USDA] Nutrition data successfully parsed!\n";
                    return nutrition;
                }
            }

            std::cout << "[USDA] No product contains all required nutrients for: "
                << ingredient_name << '\n';

            return std::nullopt;
        }
        catch (const json::exception& e) {
            std::cout << "[USDA] JSON error: "
                << e.what() << '\n';
            return std::nullopt;
        }
    }

    void calculate_dish(Dish& dish) {
     
        NutritionCalculator::calculate_dish(dish);
        std::cout << "\n-----------------------\n";

        std::cout << "Calories: "
            << dish.total_nutrition.calories << '\n';

        std::cout << "Proteins: "
            << dish.total_nutrition.proteins << '\n';

        std::cout << "Fats: "
            << dish.total_nutrition.fats << '\n';

        std::cout << "Carbohydrates: "
            << dish.total_nutrition.carbohydrates << '\n';
    }
}