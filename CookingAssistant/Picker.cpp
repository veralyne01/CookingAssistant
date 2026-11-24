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
#include "reasoner.h"


namespace Picker {
    std::vector<Dish> remove_allergens(std::vector<Dish> dishes, const User& user) {
        for (const auto& allergen : user.allergens) {
            dishes.erase(std::remove(dishes.begin(), dishes.end(), allergen), dishes.end());
        }
        return dishes;
    }

    std::vector<Dish> find_candidates(User& user, std::string category) {
        std::vector<Dish> result;
        const std::vector<std::string> ids = MealDB::filter_ids_by_category(category);

        result = remove_allergens(result, user);
        return result;
    }

    std::vector<ScoredDish> get_scored_candidates(std::vector<Dish> dishes, User& user) {
        std::vector<ScoredDish> scored;
        for (const auto& d : dishes) {
            ScoredDish dish;
            dish.dish = d;
            dish.parameter = Reasoner::total_compliance(d, user);
            scored.push_back(dish);
        }
        return scored;
    }

    std::vector<ScoredDish> find_candidates(std::vector<Dish> dishes, User& user) {
        std::vector<ScoredDish> scored = get_scored_candidates(dishes, user);
        std::stable_sort(scored.begin(), scored.end(),
            [&](const ScoredDish& a, const ScoredDish& b) {
                return a.parameter > b.parameter;
            });
        return scored;
    }
}