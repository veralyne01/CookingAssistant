#pragma once

#include "user.h"
#include "dishes.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class Reasoner {
private:
    static double meal_type_compliance(const json& f, Dish dish);
    static double user_preferences_compliance(User& user, Dish dish);
    static double calorage_compliance(User& user, Dish dish);
    static double cuisine_compliance(const json& f, Dish dish);
public:
    static double total_compliance(Dish dish, User& user);
};