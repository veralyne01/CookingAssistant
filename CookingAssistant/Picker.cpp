
#include "picker.h"

#include "profile_manager.h" 
#include "meal_db_search.h"
#include "auxiliary.h"
#include "reasoner.h"

#include <algorithm>
#include <stdexcept>
#include <unordered_set>

namespace Picker {

    std::vector<Dish> remove_allergens(
        const std::vector<Dish>& dishes,
        const User& user
    ) {
        std::vector<Dish> result;

        for (const auto& dish : dishes) {
            bool has_allergen = false;

            for (const auto& ingredient : dish.ingredients) {
                for (const auto& allergen : user.allergens) {

                    if (Auxiliary::contains_ci(
                        ingredient.name, allergen
                    )) {
                        has_allergen = true;
                        break;
                    }
                }

                if (has_allergen) {
                    break;
                }
            }

            if (!has_allergen) {
                result.push_back(dish);
            }
        }

        return result;
    }



    std::vector<Dish> find_candidates(
        const User& user,
        const std::string& category
    ) {
        std::vector<Dish> result;
        std::unordered_set<std::string> seen;

        const auto ids = MealDB::filter_ids_by_category(category);

        for (const auto& id : ids) {
            if (!seen.insert(id).second) {
                continue;
            }

            auto dish = MealDB::lookup_by_id(id);

            if (dish.has_value()) {
                result.push_back(*dish);
            }
        }

        const std::unordered_map<std::string,
            std::vector<std::string>> search_terms = {
            {"breakfast", BREAKFAST_KEYWORDS},
            {"lunch", LUNCH_KEYWORDS},
            {"dinner", DINNER_KEYWORDS},
            {"snack", SNACK_KEYWORDS}
        };

        auto terms = search_terms.find(category);
        if (terms != search_terms.end()) {
            for (const auto& term : terms->second) {

                auto found = MealDB::search_by_name(term);

                for (auto& dish : found) {
                    if (!seen.insert(dish.id).second) {
                        continue;
                    }

                    result.push_back(std::move(dish));
                }
            }
        }

        return remove_allergens(result, user);
    }



    std::vector<ScoredDish> get_scored_candidates(
        const std::vector<Dish>& dishes,
        User& user
    ) {
        std::vector<ScoredDish> scored;

        for (const auto& dish : dishes) {

            ScoredDish item;

            item.dish = dish;

            item.parameter =
                Reasoner::total_compliance(dish, user);

            scored.push_back(item);
        }

        return scored;
    }


    std::vector<ScoredDish> sort_candidates(
        std::vector<ScoredDish> dishes
    ) {
        std::stable_sort(
            dishes.begin(),
            dishes.end(),
            [](const ScoredDish& a, const ScoredDish& b) {
                return a.parameter > b.parameter;
            }
        );

        return dishes;
    }


    std::optional<Dish> find_suitable_dish(
        User& user,
        const std::string& category
    ) {
        std::vector<Dish> dishes =
            find_candidates(user, category);

        std::vector<ScoredDish> scored =
            get_scored_candidates(dishes, user);

        scored = sort_candidates(std::move(scored));

        if (scored.empty()) {
            throw std::runtime_error(
                "No suitable dishes found!"
            );
        }

        return scored.front().dish;
    }

}
