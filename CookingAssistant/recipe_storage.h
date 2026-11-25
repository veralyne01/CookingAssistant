#pragma once

#include <string>
#include <unordered_set>
#include <vector>

#include "dishes.h"

class User;

namespace RecipeStorage {

std::vector<Dish> load_liked_recipes();
void save_liked_recipe(const Dish& dish);
void mark_recipe_seen(const std::string& id);
std::unordered_set<std::string> load_menu_used_ids();
void mark_menu_recipe_used(const std::string& id);
void reset_menu_used_ids();
std::unordered_set<std::string> load_seen_recipe_ids();
void save_favorite_recipe(const Dish& dish);
std::vector<Dish> load_favorite_recipes();
void save_favorite_menu(const std::vector<Dish>& dishes);
std::vector<std::vector<Dish>> load_favorite_menus();
void like_recipe(const Dish& dish);
void dislike_recipe(const Dish& dish, User& user);

}
