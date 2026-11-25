#include "ui.h"

#include <algorithm>
#include <iostream>
#include <optional>
#include <unordered_set>
#include <utility>
#include <vector>

#include "enums.h"
#include "food_data_central.h"
#include "profile_manager.h"
#include "recipe_storage.h"

namespace UI {

std::string choose_meal_category() {
  std::cout << "Choose meal category:\n"
            << "1. Breakfast\n2. Lunch\n3. Snack\n4. Dinner\n";
  const int choice = read_menu_option(1, 4);
  static const std::vector<std::string> categories = {
      "breakfast", "lunch", "snack", "dinner"};
  return categories[choice - 1];
}

void show_result(Dish& dish, User& user, bool allow_feedback) {
  std::cout << "\n========================================\n"
            << "             " << dish.name << "\n"
            << "========================================\n";

  if (!dish.category.empty()) {
    const auto category = Values::meals.find(dish.category);
    std::cout << "CATEGORY: "
              << (category == Values::meals.end() ? dish.category
                                                   : category->second)
              << '\n';
  }
  if (!dish.area.empty()) {
    std::cout << "CUISINE: " << dish.area << '\n';
  }

  std::cout << "\nINGREDIENTS:\n";
  for (const Ingredient& ingredient : dish.ingredients) {
    std::cout << " - " << ingredient.name;
    if (!ingredient.measure.empty()) {
      std::cout << " — " << ingredient.measure;
    }
    std::cout << '\n';
  }

  FoodDataCentral::calculate_dish(dish);
  std::cout << "\nNUTRITION:\n"
            << "Calories: " << dish.total_nutrition.calories << " kcal\n"
            << "Proteins: " << dish.total_nutrition.proteins << " g\n"
            << "Fats: " << dish.total_nutrition.fats << " g\n"
            << "Carbohydrates: " << dish.total_nutrition.carbohydrates
            << " g\n";

  if (!dish.nutrition_complete) {
    std::cout << "\nWarning: nutrition data is incomplete. "
              << "Some ingredients could not be calculated.\n";
  }

  std::cout << "\nINSTRUCTIONS:\n";
  std::cout << (dish.instructions.empty()
                    ? "Instructions are not available.\n"
                    : dish.instructions + "\n");
  std::cout << "\n========================================\n";

  if (!dish.id.empty()) {
    RecipeStorage::mark_recipe_seen(dish.id);
  }
  if (!allow_feedback) {
    return;
  }

  std::cout << "Did you like this recipe?\n"
            << "1. Yes\n2. No\n3. Skip\n";
  switch (read_menu_option(1, 3)) {
    case 1:
      RecipeStorage::like_recipe(dish);
      std::cout << "Recipe saved to your local recipe database.\n";
      break;
    case 2:
      RecipeStorage::dislike_recipe(dish, user);
      ProfileManager::save_user_to_file(user);
      std::cout << "Recipe added to your disliked dishes.\n";
      break;
    default:
      break;
  }

  std::cout << "Add this dish to favorites? (1. Yes / 2. No)\n";
  if (read_menu_option(1, 2) == 1) {
    RecipeStorage::save_favorite_recipe(dish);
    std::cout << "Dish saved to favorites.\n";
  }
}

std::vector<Dish> show_daily_menu(
    User& user, std::unordered_set<std::string>* used_ids) {
  std::vector<std::optional<Dish>> selected =
      Picker::suggest_daily_menu(user, used_ids);
  std::vector<Dish> menu;
  for (std::optional<Dish>& dish : selected) {
    if (!dish.has_value()) {
      continue;
    }
    show_result(*dish, user);
    menu.push_back(std::move(*dish));
  }
  return menu;
}

void show_weekly_menu(User& user) {
  std::unordered_set<std::string> used_ids =
      RecipeStorage::load_menu_used_ids();
  const std::vector<Dish> saved_recipes = RecipeStorage::load_liked_recipes();
  std::size_t used_local_count = 0;
  for (const Dish& dish : saved_recipes) {
    if (!dish.id.empty() && used_ids.count(dish.id) > 0) {
      ++used_local_count;
    }
  }
  if (!saved_recipes.empty() && used_local_count >= saved_recipes.size()) {
    RecipeStorage::reset_menu_used_ids();
    used_ids.clear();
  }
  std::vector<Dish> weekly_menu;
  for (int day = 1; day <= 7; ++day) {
    std::cout << "\n================ DAY " << day
              << " ================\n";
    std::vector<Dish> daily_menu = show_daily_menu(user, &used_ids);
    for (const Dish& dish : daily_menu) {
      const bool is_saved = std::any_of(
          saved_recipes.begin(), saved_recipes.end(),
          [&dish](const Dish& saved) { return saved.id == dish.id; });
      if (is_saved) {
        RecipeStorage::mark_menu_recipe_used(dish.id);
      }
    }
    weekly_menu.insert(weekly_menu.end(), daily_menu.begin(), daily_menu.end());
  }

  std::cout << "Save this weekly menu to favorites? (1. Yes / 2. No)\n";
  if (read_menu_option(1, 2) == 1) {
    RecipeStorage::save_favorite_menu(weekly_menu);
    std::cout << "Weekly menu saved.\n";
  }
}

void show_saved_menus(User& user) {
  const std::vector<std::vector<Dish>> menus =
      RecipeStorage::load_favorite_menus();
  if (menus.empty()) {
    std::cout << "There are no favorite menus yet.\n";
    return;
  }

  for (std::size_t index = 0; index < menus.size(); ++index) {
    std::cout << "\nMenu " << index + 1 << ":\n";
    for (const Dish& dish : menus[index]) {
      std::cout << " - " << dish.name << " (" << dish.category << ")\n";
    }
  }
  std::cout << "Enter menu number to view it or 0 to return.\n";
  const int choice = read_menu_option(0, static_cast<int>(menus.size()));
  if (choice == 0) {
    return;
  }
  for (Dish dish : menus[choice - 1]) {
    show_result(dish, user, false);
  }
}

void show_saved_recipes(User& user) {
  const std::vector<Dish> recipes = RecipeStorage::load_favorite_recipes();
  if (recipes.empty()) {
    std::cout << "There are no favorite recipes yet.\n";
    return;
  }

  for (std::size_t index = 0; index < recipes.size(); ++index) {
    std::cout << index + 1 << ". " << recipes[index].name << '\n';
  }
  std::cout << "Enter recipe number to view it or 0 to return.\n";
  const int choice = read_menu_option(0, static_cast<int>(recipes.size()));
  if (choice == 0) {
    return;
  }
  Dish dish = recipes[choice - 1];
  show_result(dish, user, false);
}

}
