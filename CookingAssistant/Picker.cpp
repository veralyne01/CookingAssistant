#include "picker.h"
#include "meal_db_search.h"
#include "auxiliary.h"
#include <algorithm>
#include <cctype>
#include <chrono>
#include <random>
#include <thread>
#include <unordered_map>
#include <utility>
#include <utility>
#include "dishes.h"

using namespace Auxiliary;

std::vector<Dish> remove_allergens(std::vector<Dish> dishes, User& user) {
    for (const auto& target : dishes) {
        user.allergens.erase(std::remove(user.allergens.begin(), user.allergens.end(), target.name), user.allergens.end());
    }
}

//1. получить 20 результатов
//2. убрать аллергены
//3. если там не 0, оценить веса
//4. отсортировать в порядке убывания коэффициентов
//5. добавить чуть позже: 
//6. показать итоговый вариант

std::vector<Dish> Picker::find_candidates(User& user, const std::unordered_set<std::string>& already_shown, std::string category) {
    std::vector<Dish> result;
    std::unordered_set<std::string> seen = already_shown;
    constexpr std::size_t kMaxCandidates = 20;
    constexpr std::size_t kMaxLookups = 40;

    auto consider = [&](Dish dish) {
        if (dish.id.empty() || !seen.insert(dish.id).second) return;
        dish.category = category;
    };

    const std::vector<std::string> ids = MealDB::filter_ids_by_category(category);

    std::size_t lookups = 0;
    for (const auto& id : ids) {
        if (result.size() >= kMaxCandidates || lookups >= kMaxLookups) break;
        if (seen.count(id)) continue;
        std::this_thread::sleep_for(PAUSE);
        ++lookups;
        if (auto dish = MealDB::lookup_by_id(id)) consider(std::move(*dish));
    }

    const std::unordered_map<std::string, std::vector<std::string>> search_terms = {
        {"Breakfast", BREAKFAST_KEYWORDS}, {"Lunch", LUNCH_KEYWORDS},
        {"Dinner", DINNER_KEYWORDS}, {"Snack", SNACK_KEYWORDS}
    };
    auto terms = search_terms.find(category);
    if (terms != search_terms.end()) {
        for (const auto& term : terms->second) {
            if (result.size() >= kMaxCandidates) break;
            std::this_thread::sleep_for(PAUSE);
            for (auto dish : MealDB::search_by_name(term)) {
                consider(std::move(dish));
                if (result.size() >= kMaxCandidates) break;
            }
        }
    }
    if (result.size() > kMaxCandidates) result.resize(kMaxCandidates);
    result = remove_allergens(result, user);
    return result;
}
