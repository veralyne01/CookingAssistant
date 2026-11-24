#pragma once

#include "dishes.h"
#include <vector>
#include <string>

namespace Picker {

    struct ScoredDish {
        Dish dish;
        double parameter;
    };

    std::vector<Dish> remove_allergens(
        std::vector<Dish> dishes,
        const User& user
    );

    std::vector<Dish> find_candidates(
        const User& user,
        const std::string& category
    );

    std::vector<ScoredDish> get_scored_candidates(
        const std::vector<Dish>& dishes,
        const User& user
    );

    std::vector<ScoredDish> sort_candidates(
        std::vector<ScoredDish> dishes
    );
}