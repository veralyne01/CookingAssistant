#include "ui.h"
#include "mealdbsearch.h"
namespace UI {
	void menu(User& user) {
		//system("cls");
		std::cout << "-------------------------\n COOKING ASSISTANT 1.0.0\n-------------------------\n";
		std::cout << "1. Profile\n2. Quick dish selection\n3. New menu for a day\n4. New menu for a week\n5. Saved menus\n6. Search recipies\n7. Saved recipies\n8. Exit\n> ";
		char input;
		std::cin >> input;
		switch (input) {
		case '1':
			profile_menu(user);
			profile_options(user);
			break;
		case '2': {
			std::string category;
			category = choose_meal_category();
			MealDB::show_result(user, category);
			menu(user);
			break;
		}
		case '3':
			//составление меню на день
			break;
		case '4':
			//составление меню на неделю
			break;
		case '5':
			//просто вывод сохраненных меню из файлика
			break;
		case '6':
			//поиск по названию/категории блюда
			break;
		case '7':
			//вывод сохраненных рецептов блюд из файлика
			break;
		case '8': break;
		default: {
			std::cout << "Invalid input! Please, try again\n";
			menu(user);
		}
		}
	}
}