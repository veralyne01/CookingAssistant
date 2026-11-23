#include "user.h"
#include "ui.h"
#include "groups.h"
#include "enums.h"
#include <iostream>
#include <fstream> 
#include <string>
#include <cstdlib>
#include "profile_manager.h"
#include "meal_db_search.h"

namespace UI {
	void fill_gender(User& user) {
		char g;
		system("cls");
		std::cout << "Choose your gender: enter M for male or F for female\n" << "> ";
		std::cin >> g;
		if (g == 'M' || g == 'm') user.gender = "male";
		else if (g == 'F' || g == 'f') user.gender = "female";
		else {
			std::cout << "Invalid input! Please, try again\n";
			fill_gender(user);
		}
	}
	void fill_name(User& user) {
		system("cls");
		std::cout << "Please, enter your name:\n" << "> ";
		std::cin >> user.name;

	}
	void fill_target(User& user) {
		system("cls");
		char t;
		std::cout << "Choose your target:\n1. I want to lose weight\n2. I want to stay fit\n3. I want to gain weight\n" << "> ";
		std::cin >> t;
		if (t == '1') user.target = "weight_loss";
		else if (t == '2') user.target = "maintenance";
		else if (t == '3') user.target = "weight_gain";
		else fill_target(user);
	}

	void fill_weight(User& user) {
		system("cls");
		const double min_weight = 20.0;
		const double max_weight = 300.0;
		double value;

		while (true) {
			std::cout << "Enter your weight (kg):\n> ";
			if (!(std::cin >> value)) {
				if (std::cin.eof()) return;
				std::cin.clear();
				std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				std::cout << "Invalid input! Please enter a number.\n";
				continue;
			}

			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

			if (value < min_weight || value > max_weight) {
				std::cout << "Weight is out of boundaries!\n";
				continue;
			}
			user.weight = value;
			return;
		}
	}

	void fill_height(User& user) {
		system("cls");
		const double min_height = 1.0;
		const double max_height = 2.5;
		double value;

		while (true) {
			std::cout << "Enter your height (m):\n> ";
			if (!(std::cin >> value)) {
				if (std::cin.eof()) return;
				std::cin.clear();
				std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				std::cout << "Invalid input! Please, try again\n";
				continue;
			}

			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

			if (value < min_height || value > max_height) {
				std::cout << "Height is out of boundaries!\n";
				continue;
			}
			user.height = value;
			return;
		}
	}

	void fill_age(User& user) {
		system("cls");
		const int min_age = 1;
		const int max_age = 100;
		int value;

		while (true) {
			std::cout << "Enter your age:\n> ";
			if (!(std::cin >> value)) {
				if (std::cin.eof()) return;
				std::cin.clear();
				std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				std::cout << "Invalid input! Please, try again\n";
				continue;
			}

			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

			if (value < min_age || value > max_age) {
				std::cout << "Age is out of boundaries!\n";
				continue;
			}
			user.age = value;
			return;
		}
	}
	void fill_activity_level(User& user) {
		system("cls");
		char t;
		std::cout << "Choose your activity level:\n1. I do desk work and don't go to workouts\n2. I go to moderate-intensity workouts 1-2 times a week\n" <<
			"3. I go to moderate-intensity workouts 3-5 times a week\n4. I train intensely almost every day or do physical labor\n" <<
			"5. I train every day and work in a physically demanding job\n" << "> ";
		std::cin >> t;
		if (t == '1') user.activity_level = "no_activity";
		else if (t == '2') user.activity_level = "light_exertion";
		else if (t == '3') user.activity_level = "regular_workouts";
		else if (t == '4') user.activity_level = "frequent_workouts";
		else if (t == '5') user.activity_level = "hard_work";
		else fill_activity_level(user);
	}

	void ask_calorie_norm(User& user) {
		int cal = User::count_target_calories(user);
		std::cout << "\nSuggested calorie norm: " << cal << ". Would you like to change it (Y/N)?\n> ";
		char g;
		std::cin >> g;
		if (g == 'Y' || g == 'y') fill_calorie_norm(user, cal);
		else if (g == 'N' || g == 'n') {
			user.calorie_norm = cal;
			std::cout << "Calorie norm saved!\n";
		}
		else {
			std::cout << "Invalid input! Please, try again\n";
			std::cin >> g;
			ask_calorie_norm(user);
		}
	}

	void fill_calorie_norm(User& user, int cal) {
		system("cls");
		int value;

		while (true) {
			std::cout << "Enter calorie norm:\n> ";
			if (!(std::cin >> value)) {
				if (std::cin.eof()) return;
				std::cin.clear();
				std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				std::cout << "Invalid input! Please, try again\n";
				continue;
			}

			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

			if (value < cal) {
				std::cout << "An excessively low daily calorie intake can have a negative impact on your health. Do you really want to continue (Y/N)?\n";
				char g;
				std::cin >> g;
				if (g == 'Y' || g == 'y') user.calorie_norm = value;
				else if (g == 'N' || g == 'n') continue;
				else {
					std::cout << "Invalid input! Please, try again\n";
					continue;
				}
			}
			if (value <= 0) {
				std::cout << "Invalid input! Please, try again\n";
				continue;
			}
			user.calorie_norm = value;
			return;
		}
	}

	void choose_allergies(User& user) {
		system("cls");
		std::cout << "Choose your allergies (Enter the numbers separated by a space):\n" <<
			"1. Nuts\n2. Peanuts\n3. Dairy\n" <<
			"4. Eggs\n5. Gluten\n6. Seafood\n" <<
			"7. Soy\n8. Chicken\n9. Another\n> ";
		std::string input;

	}

	void profile_menu(User& user) {
		system("cls");
		ProfileManager::read_user_from_file(user);
		std::cout << "------------\n MY PROFILE\n------------\n";
		std::cout << "Name:\t\t";
		(user.name.find_first_not_of(" \n\t\r") == std::string::npos) ? std::cout << "-\n" : std::cout << user.name << "\n";
		std::cout << "Age:\t\t";
		(user.age == 0) ? std::cout << "-\n" : std::cout << user.age << "\n";
		std::cout << "Gender:\t\t" << Values::genders.at(user.gender) << "\n";
		std::cout << "Weight:\t\t";
		(user.weight == 0) ? std::cout << "-\n" : std::cout << user.weight << "\n";
		std::cout << "Height:\t\t";
		(user.height == 0) ? std::cout << "-\n" : std::cout << user.height << "\n";
		std::cout << "Target:\t\t" << Values::targets.at(user.target) << "\n";
		std::cout << "Activity level:\t" << Values::activity_levels.at(user.activity_level) << "\n";
		std::cout << "Calorie norm:\t";
		(user.calorie_norm == 0) ? std::cout << "-\n" : std::cout << user.calorie_norm << "\n";
	}

	void input_profile(User& user) {
		fill_name(user);
		fill_age(user);
		fill_gender(user);
		fill_weight(user);
		fill_height(user);
		fill_target(user);
		fill_activity_level(user);
		ask_calorie_norm(user);
		ProfileManager::save_user_to_file(user);
	}

    void edit_profile(User& user) {
        std::cout << "Choose option to edit:\n1. Name\n2. Age\n3. Gender\n4. Weight\n5. Height\n6. Target\n7. Activity level\n";
        switch (read_menu_option(1, 7)) {
        case 1: fill_name(user); break;
        case 2: fill_age(user); ask_calorie_norm(user); break;
        case 3: fill_gender(user); ask_calorie_norm(user); break;
        case 4: fill_weight(user); ask_calorie_norm(user); break;
        case 5: fill_height(user); ask_calorie_norm(user); break;
        case 6: fill_target(user); ask_calorie_norm(user); break;
        case 7: fill_activity_level(user); ask_calorie_norm(user); break;
        }
        ProfileManager::save_user_to_file(user);
    }

    void profile_options(User& user) {
        std::cout << "\n1. New profile\n2. Edit profile\n3. Back\n";
        switch (read_menu_option(1, 3)) {
        case 1: input_profile(user); break;
        case 2: edit_profile(user); break;
        case 3: return;
        }
    }

}