#include "ui.h"

#include <exception>
#include <iostream>
#include <string>

#include "profile_manager.h"
#include "recipe_storage.h"

namespace UI {

int read_menu_option(int min_value, int max_value) {
  while (true) {
    std::string input;
    std::cout << "> ";
    if (!std::getline(std::cin, input)) {
      return max_value;
    }
    if (input.empty()) {
      continue;
    }

    try {
      std::size_t parsed = 0;
      const int value = std::stoi(input, &parsed);
      if (parsed == input.size() && value >= min_value && value <= max_value) {
        return value;
      }
    } catch (const std::exception&) {
    }
    std::cout << "Invalid input! Please, try again\n";
  }
}

void menu(User& user) {
  ProfileManager::read_user_from_file(user);
  while (true) {
    std::cout << "-------------------------\n"
              << " COOKING ASSISTANT 1.0.0\n"
              << "-------------------------\n"
              << "1. Profile\n"
              << "2. Quick dish selection\n"
              << "3. New menu for a day\n"
              << "4. New menu for a week\n"
              << "5. Saved menus\n"
              << "6. Search recipes\n"
              << "7. Saved recipes\n"
              << "8. Exit\n";

    switch (read_menu_option(1, 8)) {
      case 1:
        profile_menu(user);
        profile_options(user);
        break;
      case 2: {
        const std::string category = choose_meal_category();
        std::optional<Dish> dish =
            Picker::find_suitable_dish(user, category, false);
        if (dish.has_value()) {
          show_result(*dish, user);
        } else {
          std::cout << "No new suitable recipes were found.\n";
        }
        break;
      }
      case 3: {
        std::vector<Dish> daily_menu = show_daily_menu(user);
        std::cout << "Save this menu to favorites? (1. Yes / 2. No)\n";
        if (read_menu_option(1, 2) == 1) {
          RecipeStorage::save_favorite_menu(daily_menu);
          std::cout << "Menu saved.\n";
        }
        break;
      }
      case 4:
        show_weekly_menu(user);
        break;
      case 5:
        show_saved_menus(user);
        break;
      case 6:
        std::cout << "Use quick dish selection to get a new recipe.\n";
        break;
      case 7:
        show_saved_recipes(user);
        break;
      case 8:
        ProfileManager::save_user_to_file(user);
        return;
    }
  }
}

}
