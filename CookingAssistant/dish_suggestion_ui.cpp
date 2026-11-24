#include "ui.h"
#include "dishes.h"
#include "meal_db_search.h"
#include "food_data_central.h"
#include "enums.h"
#include <vector>

namespace UI {
    std::string choose_meal_category() {
        std::cout << "Choose meal category:\n1. Breakfast\n2. Lunch\n3. Snack\n4. Dinner\n";
        const int choice = read_menu_option(1, 4);
        static const std::vector<std::string> categories = {"breakfast", "lunch", "snack", "dinner"};
        return categories[choice - 1];
    }
    void show_result(User& user, std::string category) {

        auto dish = Picker::find_suitable_dish(user, category);

        if (!dish) {
            std::cout << "\nNo suitable meal found.\n";
            return;
        }

        std::cout << "\n========================================\n";
        std::cout << "             " << dish->name << "\n";
        std::cout << "========================================\n";

        if (!dish->category.empty())
            std::cout << "CATEGORY: " << Values::meals.at(dish->category) << "\n";

        if (!dish->area.empty())
            std::cout << "CUISINE: " << dish->area << "\n";

        std::cout << "\nINGREDIENTS:\n";

        for (size_t i = 0; i < dish->ingredients.size(); ++i) {
            std::cout << " - " << dish->ingredients[i].name;

            if (i < dish->ingredients.size() &&
                !dish->ingredients[i].measure.empty()) {
                std::cout << " — " << dish->ingredients[i].measure;
            }

            std::cout << "\n";
        }
        FoodDataCentral::calculate_dish(*dish);
        std::cout << "\nNUTRITION:\n";

        std::cout << "Calories: "
            << dish->total_nutrition.calories << " kcal\n";

        std::cout << "Proteins: "
            << dish->total_nutrition.proteins << " g\n";

        std::cout << "Fats: "
            << dish->total_nutrition.fats << " g\n";

        std::cout << "Carbohydrates: "
            << dish->total_nutrition.carbohydrates << " g\n";

        if (!dish->nutrition_complete) {
            std::cout << "\nWarning: nutrition data is incomplete. "
                << "Some ingredients could not be calculated.\n";
        }

        std::cout << "\nINSTRUCTIONS:\n";

        if (!dish->instructions.empty()) {
            std::cout << dish->instructions << "\n";
        }
        else {
            std::cout << "Instructions are not available.\n";
        }

        std::cout << "\n========================================\n";
    }

}