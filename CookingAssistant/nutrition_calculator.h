
#pragma once

#include "dishes.h"

#include <vector>

namespace NutritionCalculator {

    Nutrition calculate_ingredient(
        double weight_grams,
        const Nutrition& per_100g
    );

    Nutrition calculate_total(
        const std::vector<Ingredient>& ingredients
    );

    Nutrition calculate_per_serving(
        const Nutrition& total,
        int servings
    );

    void calculate_dish(Dish& dish);
}
