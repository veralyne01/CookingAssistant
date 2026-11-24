#include "ui.h"
#include "meal_db_search.h"
#include <string>
#include <stdexcept>

namespace UI {
    int read_menu_option(int min_value, int max_value) {
        while (true) {
            std::string input;
            std::cout << "> ";
            if (!std::getline(std::cin, input)) return max_value;
            try {
                std::size_t parsed = 0;
                const int value = std::stoi(input, &parsed);
                if (parsed == input.size() && value >= min_value && value <= max_value) {
                    return value;
                }
            }
            catch (const std::exception&) {
            }
            std::cout << "Invalid input! Please, try again\n";
        }
    }

    void menu(User& user) {
        while (true) {
            std::cout << "-------------------------\n COOKING ASSISTANT 1.0.0\n-------------------------\n"
                "1. Profile\n2. Quick dish selection\n3. New menu for a day\n4. New menu for a week\n5. Saved menus\n6. Search recipes\n7. Saved recipes\n8. Exit\n";
            switch (read_menu_option(1, 8)) {
            case 1: profile_menu(user); profile_options(user); break;
            case 2: show_result(user, choose_meal_category()); break;
            case 3: std::cout << "Daily menu is not implemented yet.\n"; break;
            case 4: std::cout << "Weekly menu is not implemented yet.\n"; break;
            case 5: std::cout << "Saved menus are not implemented yet.\n"; break;
            case 6: std::cout << "Recipe search is not implemented yet.\n"; break;
            case 7: std::cout << "Saved recipes are not implemented yet.\n"; break;
            case 8: return;
            }
        }
    }
}