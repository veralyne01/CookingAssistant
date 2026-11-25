#pragma once

#include <string>
#include <unordered_set>
#include <utility>
#include <vector>

#include "dishes.h"
#include "picker.h"
#include "user.h"

#include <nlohmann/json.hpp>

namespace UI {

void fill_gender(User& user);
void fill_name(User& user);
void fill_target(User& user);
void fill_weight(User& user);
void fill_height(User& user);
void fill_activity_level(User& user);
void ask_calorie_norm(User& user);
void fill_calorie_norm(User& user, int calories);
void menu(User& user);
int read_menu_option(int min_value, int max_value);
void profile_menu(User& user);
void fill_age(User& user);
void input_profile(User& user);
void edit_profile(User& user);
void profile_options(User& user);
std::vector<std::string> choose_categories(
    const std::string& section,
    const std::vector<std::pair<std::string, std::string>>& options);
std::vector<std::string> add_category_from_json(
    std::vector<std::string> list, const std::string& section,
    const std::string& category, const nlohmann::json& rules);
std::string choose_meal_category();
void show_result(Dish& dish, User& user, bool allow_feedback = true);
std::vector<std::string> choose_allergens();
std::vector<std::string> choose_products();
std::vector<Dish> show_daily_menu(
    User& user, std::unordered_set<std::string>* used_ids = nullptr);
void show_weekly_menu(User& user);
void show_saved_menus(User& user);
void show_saved_recipes(User& user);

}
