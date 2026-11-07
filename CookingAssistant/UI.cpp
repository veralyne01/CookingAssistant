#include "user.h"
#include "ui.h"
#include "enums.h"
#include <iostream>
#include <fstream> 
#include <string>
#include <cstdlib>
#include "ProfileManager.h"

void UI::fill_gender(User& user) {
	char g;
	system("cls");
	std::cout << "Choose your gender: enter M for male or F for female\n" << "> ";
	std::cin >> g;
	if (g == 'M' || g == 'm') user.gender = Gender::male;
	else if (g == 'F' || g == 'f') user.gender = Gender::female;
	else { 
		std::cout << "Invalid input! Please, try again\n";
		std::cin >> g;
		fill_gender(user); 
	}
}
void UI::fill_name(User& user) {
	system("cls");
	std::cout << "Please, enter your name:\n" << "> ";
	std::cin >> user.name;

}
void UI::fill_target(User& user) {
	system("cls");
	char t;
	std::cout << "Choose your target:\n1. I want to lose weight\n2. I want to stay fit\n3. I want to gain weight\n" << "> ";
	std::cin >> t;
	if (t == '1') user.target = Target::weight_loss;
	else if (t == '2') user.target = Target::maintenance;
	else if (t == '3') user.target = Target::weight_gain;
	else fill_target(user);
}

void UI::fill_weight(User& user) {
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

void UI::fill_height(User & user) {
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

void UI::fill_age(User& user) {
	system("cls");
	const double min_age = 1.0;
	const double max_age = 100;
	double value;

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

void UI::menu(User& user) {
	system("cls");
	std::cout << "-------------------------\n COOKING ASSISTANT 1.0.0\n-------------------------\n";
	std::cout << "1. Profile\n2. Quick dish selection\n3. New menu for a day\n4. New menu for a week\n5. Saved menus\n6. Search recipies\n7. Saved recipies\n8. Exit\n> ";
	char input;
	std::cin >> input;
	switch (input) {
	case '1':
		profile_menu(user);
		profile_options(user);
		break;
	case '2':
		//тут будет предложение одного блюда
		break;
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
void UI::profile_menu(User& user) {
	system("cls");
	ProfileManager::read_user_from_file(user);
	std::cout << "------------\n MY PROFILE\n------------\n";
	std::cout << "Name:\t";
	(user.name.find_first_not_of(" \n\t\r") == std::string::npos) ? std::cout << "-\n" : std::cout << user.name << "\n";
	std::cout << "Age:\t";
	(user.age == 0) ? std::cout << "-\n" : std::cout << user.age << "\n";
	std::cout << "Gender:\t" << ProfileManager::parse_gender(user.gender) << "\n";
	std::cout << "Weight:\t";
	(user.weight == 0) ? std::cout << "-\n" : std::cout << user.weight << "\n";
	std::cout << "Height:\t";
	(user.height == 0) ? std::cout << "-\n" : std::cout << user.height << "\n";
	std::cout << "Target:\t" << ProfileManager::parse_target(user.target) << "\n";
}

void UI::input_profile(User& user) {
	fill_name(user);
	fill_age(user);
	fill_gender(user);
	fill_weight(user);
	fill_height(user);
	fill_target(user);
	ProfileManager::save_user_to_file(user);
}

void UI::edit_profile(User& user) {
	std::cout << "Choose option to edit:\n1. Name\n2. Age\n3. Gender\n4. Weight\n5. Height\n6. Target\n";
	char c;
	std::cin >> c;
	switch (c) {
	case '1':
		fill_name(user);
		break;
	case '2':
		fill_age(user);
		break;
	case '3':
		fill_gender(user);
		break;
	case '4':
		fill_weight(user);
		break;
	case '5':
		fill_height(user);
		break;
	case '6':
		fill_target(user);
		break;
	default:
		std::cout << "Invalid input! Please, try again\n";
		break;
	}
	ProfileManager::save_user_to_file(user);
}

void UI::profile_options(User& user) {
	std::cout << "\n1. New profile\n2. Edit profile\n3. Back\n";
	char c;
	std::cin >> c;
	switch (c) {
	case '1': {
		input_profile(user);
		menu(user);
		break;
	}
	case '2': {
		edit_profile(user);
		menu(user);
		break;
	}
	case '3': {
		menu(user);
		break;
	}
	}
}