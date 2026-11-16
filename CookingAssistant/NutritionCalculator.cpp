
#include "NutritionCalculator.h"

namespace NutritionCalculator {

    Nutrition calculate_ingredient(
        double weight_grams,
        const Nutrition& per_100g
    ) {
        const double factor = weight_grams / 100.0;

        return {
            per_100g.calories * factor,
            per_100g.proteins * factor,
            per_100g.fats * factor,
            per_100g.carbohydrates * factor
        };
    }

    Nutrition calculate_total(
        const std::vector<Ingredient>& ingredients
    ) {
        Nutrition total{};

        for (const auto& ingredient : ingredients) {
            if (!ingredient.nutrition_calculated)
                continue;

            total.calories +=
                ingredient.nutrition_for_amount.calories;

            total.proteins +=
                ingredient.nutrition_for_amount.proteins;

            total.fats +=
                ingredient.nutrition_for_amount.fats;

            total.carbohydrates +=
                ingredient.nutrition_for_amount.carbohydrates;
        }

        return total;
    }

    Nutrition calculate_per_serving(
        const Nutrition& total,
        int servings
    ) {
        if (servings <= 0)
            return {};

        return {
            total.calories / servings,
            total.proteins / servings,
            total.fats / servings,
            total.carbohydrates / servings
        };
    }

    void calculate_dish(Dish& dish) {
        dish.total_nutrition = calculate_total(dish.ingredients);

        dish.nutrition_per_serving =
            calculate_per_serving(
                dish.total_nutrition,
                dish.servings
            );

        dish.nutrition_complete = true;

        for (const auto& ingredient : dish.ingredients) {
            if (!ingredient.nutrition_calculated ||
                !ingredient.weight_grams.has_value()) {

                dish.nutrition_complete = false;
                break;
            }
        }
    }
}
