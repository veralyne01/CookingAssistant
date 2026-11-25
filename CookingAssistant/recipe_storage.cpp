#include "recipe_storage.h"

#include <algorithm>
#include <fstream>
#include <unordered_set>
#include <utility>

#include <nlohmann/json.hpp>

#include "user.h"

namespace RecipeStorage {
namespace {

using json = nlohmann::json;

const char kLocalRecipesFile[] = "local_recipes.json";
const char kFavoritesFile[] = "favorites.json";

json read_json(const std::string& filename, const json& fallback) {
  std::ifstream file(filename);
  if (!file.is_open()) {
    return fallback;
  }

  try {
    return json::parse(file);
  } catch (const json::exception&) {
    return fallback;
  }
}

void write_json(const std::string& filename, const json& value) {
  std::ofstream file(filename);
  if (file.is_open()) {
    file << value.dump(2);
  }
}

json dish_to_json(const Dish& dish) {
  json ingredients = json::array();
  for (const Ingredient& ingredient : dish.ingredients) {
    ingredients.push_back({
        {"name", ingredient.name},
        {"measure", ingredient.measure},
        {"weight_grams", ingredient.weight_grams
                             ? json(*ingredient.weight_grams)
                             : json(nullptr)},
        {"weight_is_estimated", ingredient.weight_is_estimated},
        {"nutrition_per_100g",
         {{"calories", ingredient.nutrition_per_100g.calories},
          {"proteins", ingredient.nutrition_per_100g.proteins},
          {"fats", ingredient.nutrition_per_100g.fats},
          {"carbohydrates", ingredient.nutrition_per_100g.carbohydrates}}},
        {"nutrition_for_amount",
         {{"calories", ingredient.nutrition_for_amount.calories},
          {"proteins", ingredient.nutrition_for_amount.proteins},
          {"fats", ingredient.nutrition_for_amount.fats},
          {"carbohydrates", ingredient.nutrition_for_amount.carbohydrates}}},
        {"nutrition_calculated", ingredient.nutrition_calculated}});
  }

  return {{"id", dish.id},
          {"name", dish.name},
          {"category", dish.category},
          {"area", dish.area},
          {"instructions", dish.instructions},
          {"cuisine", dish.cuisine},
          {"ingredients", ingredients},
          {"servings", dish.servings},
          {"total_nutrition",
           {{"calories", dish.total_nutrition.calories},
            {"proteins", dish.total_nutrition.proteins},
            {"fats", dish.total_nutrition.fats},
            {"carbohydrates", dish.total_nutrition.carbohydrates}}},
          {"nutrition_complete", dish.nutrition_complete}};
}

Nutrition nutrition_from_json(const json& value) {
  Nutrition nutrition;
  nutrition.calories = value.value("calories", 0.0);
  nutrition.proteins = value.value("proteins", 0.0);
  nutrition.fats = value.value("fats", 0.0);
  nutrition.carbohydrates = value.value("carbohydrates", 0.0);
  return nutrition;
}

Dish dish_from_json(const json& value) {
  Dish dish;
  dish.id = value.value("id", std::string{});
  dish.name = value.value("name", std::string{});
  dish.category = value.value("category", std::string{});
  dish.area = value.value("area", std::string{});
  dish.instructions = value.value("instructions", std::string{});
  dish.cuisine = value.value("cuisine", std::string{});
  dish.servings = value.value("servings", 1);
  dish.nutrition_complete = value.value("nutrition_complete", false);

  if (value.contains("total_nutrition") && value.at("total_nutrition").is_object()) {
    dish.total_nutrition = nutrition_from_json(value.at("total_nutrition"));
  }

  if (value.contains("ingredients") && value.at("ingredients").is_array()) {
    for (const json& item : value.at("ingredients")) {
      Ingredient ingredient;
      ingredient.name = item.value("name", std::string{});
      ingredient.measure = item.value("measure", std::string{});
      if (item.contains("weight_grams") && !item.at("weight_grams").is_null()) {
        ingredient.weight_grams = item.at("weight_grams").get<double>();
      }
      ingredient.weight_is_estimated = item.value("weight_is_estimated", false);
      ingredient.nutrition_calculated = item.value("nutrition_calculated", false);
      if (item.contains("nutrition_per_100g")) {
        ingredient.nutrition_per_100g = nutrition_from_json(item.at("nutrition_per_100g"));
      }
      if (item.contains("nutrition_for_amount")) {
        ingredient.nutrition_for_amount = nutrition_from_json(item.at("nutrition_for_amount"));
      }
      dish.ingredients.push_back(std::move(ingredient));
    }
  }

  return dish;
}

bool same_dish(const Dish& first, const Dish& second) {
  if (!first.id.empty() && !second.id.empty()) {
    return first.id == second.id;
  }
  return !first.name.empty() && first.name == second.name;
}

json load_local_data() {
  return read_json(kLocalRecipesFile,
                   {{"liked_recipes", json::array()},
                    {"seen_recipe_ids", json::array()},
                    {"menu_used_recipe_ids", json::array()}});
}

json load_favorites_data() {
  return read_json(kFavoritesFile,
                   {{"recipes", json::array()}, {"menus", json::array()}});
}

void append_unique_dish(json& array, const Dish& dish) {
  for (const json& item : array) {
    if (same_dish(dish_from_json(item), dish)) {
      return;
    }
  }
  array.push_back(dish_to_json(dish));
}

}

std::vector<Dish> load_liked_recipes() {
  const json data = load_local_data();
  std::vector<Dish> dishes;
  if (!data.contains("liked_recipes") || !data.at("liked_recipes").is_array()) {
    return dishes;
  }

  for (const json& item : data.at("liked_recipes")) {
    if (item.is_object()) {
      dishes.push_back(dish_from_json(item));
    }
  }
  return dishes;
}

void save_liked_recipe(const Dish& dish) {
  json data = load_local_data();
  if (!data.contains("liked_recipes") || !data.at("liked_recipes").is_array()) {
    data["liked_recipes"] = json::array();
  }
  append_unique_dish(data["liked_recipes"], dish);
  write_json(kLocalRecipesFile, data);
}

void mark_recipe_seen(const std::string& id) {
  if (id.empty()) {
    return;
  }
  json data = load_local_data();
  if (!data.contains("seen_recipe_ids") || !data.at("seen_recipe_ids").is_array()) {
    data["seen_recipe_ids"] = json::array();
  }
  for (const json& item : data["seen_recipe_ids"]) {
    if (item.is_string() && item.get<std::string>() == id) {
      return;
    }
  }
  data["seen_recipe_ids"].push_back(id);
  write_json(kLocalRecipesFile, data);
}



std::unordered_set<std::string> load_seen_recipe_ids() {
  const json data = load_local_data();
  std::unordered_set<std::string> ids;
  if (!data.contains("seen_recipe_ids") ||
      !data.at("seen_recipe_ids").is_array()) {
    return ids;
  }
  for (const json& item : data.at("seen_recipe_ids")) {
    if (item.is_string()) {
      ids.insert(item.get<std::string>());
    }
  }
  return ids;
}

std::unordered_set<std::string> load_menu_used_ids() {
  const json data = load_local_data();
  std::unordered_set<std::string> ids;
  if (!data.contains("menu_used_recipe_ids") ||
      !data.at("menu_used_recipe_ids").is_array()) {
    return ids;
  }
  for (const json& item : data.at("menu_used_recipe_ids")) {
    if (item.is_string()) {
      ids.insert(item.get<std::string>());
    }
  }
  return ids;
}

void mark_menu_recipe_used(const std::string& id) {
  if (id.empty()) {
    return;
  }
  json data = load_local_data();
  if (!data.contains("menu_used_recipe_ids") ||
      !data.at("menu_used_recipe_ids").is_array()) {
    data["menu_used_recipe_ids"] = json::array();
  }
  for (const json& item : data["menu_used_recipe_ids"]) {
    if (item.is_string() && item.get<std::string>() == id) {
      return;
    }
  }
  data["menu_used_recipe_ids"].push_back(id);
  write_json(kLocalRecipesFile, data);
}

void reset_menu_used_ids() {
  json data = load_local_data();
  data["menu_used_recipe_ids"] = json::array();
  write_json(kLocalRecipesFile, data);
}


void save_favorite_recipe(const Dish& dish) {
  json data = load_favorites_data();
  if (!data.contains("recipes") || !data.at("recipes").is_array()) {
    data["recipes"] = json::array();
  }
  append_unique_dish(data["recipes"], dish);
  write_json(kFavoritesFile, data);
}

std::vector<Dish> load_favorite_recipes() {
  const json data = load_favorites_data();
  std::vector<Dish> dishes;
  if (!data.contains("recipes") || !data.at("recipes").is_array()) {
    return dishes;
  }
  for (const json& item : data.at("recipes")) {
    if (item.is_object()) {
      dishes.push_back(dish_from_json(item));
    }
  }
  return dishes;
}

void save_favorite_menu(const std::vector<Dish>& dishes) {
  json data = load_favorites_data();
  if (!data.contains("menus") || !data.at("menus").is_array()) {
    data["menus"] = json::array();
  }
  json menu = json::array();
  for (const Dish& dish : dishes) {
    menu.push_back(dish_to_json(dish));
  }
  data["menus"].push_back(std::move(menu));
  write_json(kFavoritesFile, data);
}

std::vector<std::vector<Dish>> load_favorite_menus() {
  const json data = load_favorites_data();
  std::vector<std::vector<Dish>> menus;
  if (!data.contains("menus") || !data.at("menus").is_array()) {
    return menus;
  }
  for (const json& menu_json : data.at("menus")) {
    if (!menu_json.is_array()) {
      continue;
    }
    std::vector<Dish> menu;
    for (const json& item : menu_json) {
      if (item.is_object()) {
        menu.push_back(dish_from_json(item));
      }
    }
    menus.push_back(std::move(menu));
  }
  return menus;
}

void like_recipe(const Dish& dish) {
  save_liked_recipe(dish);
}

void dislike_recipe(const Dish& dish, User& user) {
  const std::string key = dish.id.empty() ? dish.name : dish.id;
  if (key.empty()) {
    return;
  }
  const bool exists = std::any_of(user.disliked_dishes.begin(),
                                  user.disliked_dishes.end(),
                                  [&dish, &key](const std::string& value) {
                                    return value == key || value == dish.name;
                                  });
  if (!exists) {
    user.disliked_dishes.push_back(key);
  }
}

}
