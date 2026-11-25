#pragma once

#include "user.h"
#include "dishes.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class Reasoner {
private:
    static double meal_type_compliance(
        const json& j,
        const Dish& dish,
        const std::string& meal_type
    );
    static double user_preferences_compliance(
        const User& user,
        const Dish& dish
    );
    static double calorage_compliance(
        const User& user,
        const Dish& dish
    );
    static double cuisine_compliance(const json& f, const Dish& dish);
public:
    static double total_compliance(
        const Dish& dish,
        const User& user,
        const std::string& meal_type
    );
};