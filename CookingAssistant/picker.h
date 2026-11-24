#pragma once
#include <optional>
#include <string>
#include <unordered_set>
#include <vector>
#include "dishes.h"
#include "user.h"

namespace Picker {
    std::vector<Dish> remove_allergens(std::vector<Dish> dishes, User& user);
    std::vector<Dish> Picker::find_candidates(User& user, const std::unordered_set<std::string>& already_shown, std::string category);
}