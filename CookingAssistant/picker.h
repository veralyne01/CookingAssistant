
#pragma once

#include "dishes.h"

#include <string>
#include <vector>

class User;

namespace Picker {

    const std::vector<std::string> BREAKFAST_KEYWORDS = {
       "pancake", "omelette", "porridge", "waffle", "toast", "oatmeal", "crepe", "granola", "cottage"
    };
    const std::vector<std::string> LUNCH_KEYWORDS = {
        "soup", "fish", "chicken", "beef", "pasta", "salad", "rice", "curry"
    };
    const std::vector<std::string> DINNER_KEYWORDS = {
        "chicken", "beef", "fish", "pasta", "curry", "stew", "roast", "casserole"
    };
    const std::vector<std::string> SNACK_KEYWORDS = {
        "sandwich", "salad", "toast", "smoothie", "fruit", "muffin", "snack", "cake"
    };

    struct ScoredDish {
        Dish dish;
        double parameter;
    };

    std::vector<Dish> remove_allergens(
        const std::vector<Dish>& dishes,
        const User& user
    );

    std::vector<Dish> find_candidates(
        const User& user,
        const std::string& category
    );

    std::vector<ScoredDish> get_scored_candidates(
        const std::vector<Dish>& dishes,
        User& user
    );

    std::vector<ScoredDish> sort_candidates(
        std::vector<ScoredDish> dishes
    );

    std::optional<Dish> find_suitable_dish(
        User& user,
        const std::string& category
    );
}
