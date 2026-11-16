
#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <optional>

#include "enums.h"

struct Nutrition {
    double calories = 0.0;
    double proteins = 0.0;
    double fats = 0.0;
    double carbohydrates = 0.0;
};

struct Ingredient {
    std::string name;
    std::string measure;

    std::optional<double> weight_grams;
    bool weight_is_estimated = false;

    Nutrition nutrition_per_100g;
    Nutrition nutrition_for_amount;

    bool nutrition_calculated = false;
};

struct Dish {
    std::string id;
    std::string name;
    std::string category;
    std::string area;
    std::string instructions;

    std::vector<Ingredient> ingredients;

    int servings = 1;

    Nutrition total_nutrition;
    Nutrition nutrition_per_serving;

    bool nutrition_complete = false;
};
