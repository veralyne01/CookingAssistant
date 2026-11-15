#include "parsers.h"
#include "ProfileManager.h"
#include <fstream>
#include <string>
#include "user.h"
void ProfileManager::save_user_to_file(User user) {
	std::ofstream profile;
	profile.open("profile.txt");
	if (profile.is_open()) {
		profile << user.name << "\n";
		profile << user.age << "\n";
		profile << Parser::parse_gender(user.gender) << "\n";
		profile << user.weight << "\n";
		profile << user.height << "\n";
		profile << Parser::parse_target(user.target) << "\n";
		profile << Parser::parse_activity_level(user.activity_level) << "\n";
		profile << user.calorie_norm << "\n";
		std::cout << "Profile saved!\n";
	}
	else std::cout << "Failed to save profile!\n";
	profile.close();
}

void ProfileManager::read_user_from_file(User& user) {
	std::ifstream profile;
	profile.open("profile.txt");
	if (profile.is_open()) {
		std::getline(profile, user.name);
		std::string age;
		std::getline(profile, age);
		user.age = stoi(age);
		std::string gender;
		std::getline(profile, gender);
		user.gender = Parser::stog(gender);
		std::string weight;
		std::getline(profile, weight);
		user.weight = stod(weight);
		std::string height;
		std::getline(profile, height);
		user.height = stod(height);
		std::string target;
		std::getline(profile, target);
		user.target = Parser::stot(target);
		std::string activity_level;
		std::getline(profile, activity_level);
		user.activity_level = Parser:: stoa(activity_level);
		std::string cal;
		std::getline(profile, cal);
		user.calorie_norm = stoi(cal);
		std::cout << "Profile loaded successfully!\n";
	}
	else std::cout << "Failed to open profile!\n";
	profile.close();
}
