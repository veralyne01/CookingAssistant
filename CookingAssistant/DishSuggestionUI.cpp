#include "ui.h"
#include "enums.h"
#include "dishes.h"
#include "mealdbsearch.h"
namespace UI {
	std::string choose_meal_category() {
		std::cout << "Choose meal category:\n1. Breakfast\n2. Lunch\n3. Snack\n4. Dinner\n> ";
		char input = 0;
		while (input == 0) {
			std::cin >> input;
			switch (input) {
			case '1': return "Breakfast";
			case '2': return "Lunch";
			case '3': return "Snack";
			case '4': return "Dinner";
			default:
				std::cout << "Invalid input! Please, try again\n";
				input = 0;
			}
		}
	}
}