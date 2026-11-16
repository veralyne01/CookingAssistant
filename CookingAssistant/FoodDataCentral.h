
#pragma once

#include <optional>
#include <string>
#include "dishes.h"

namespace FoodDataCentral {

    std::optional<Nutrition> get_nutrition(
        const std::string& ingredient_name
    );
    void calculate_dish(Dish& dish);
}
