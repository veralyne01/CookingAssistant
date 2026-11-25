#pragma once

#include <optional>
#include <string>
#include <unordered_set>
#include <vector>

#include "dishes.h"

class User;

namespace Picker {

extern const std::vector<std::string> BREAKFAST_KEYWORDS;
extern const std::vector<std::string> LUNCH_KEYWORDS;
extern const std::vector<std::string> DINNER_KEYWORDS;
extern const std::vector<std::string> SNACK_KEYWORDS;

struct ScoredDish {
  Dish dish;
  double parameter = 0.0;
};

std::vector<Dish> remove_allergens(const std::vector<Dish>& dishes,
                                   const User& user);
std::vector<Dish> find_candidates(const User& user,
                                  const std::string& category,
                                  bool include_local = true,
                                  const std::unordered_set<std::string>& used_ids = {});
std::vector<ScoredDish> get_scored_candidates(const std::vector<Dish>& dishes,
                                               const User& user,
                                               const std::string& category);
std::vector<ScoredDish> sort_candidates(std::vector<ScoredDish> dishes);
std::optional<Dish> find_suitable_dish(
    User& user, const std::string& category, bool include_local = true,
    const std::unordered_set<std::string>& used_ids = {});
std::vector<std::optional<Dish>> suggest_daily_menu(
    User& user, std::unordered_set<std::string>* used_ids = nullptr);

}
