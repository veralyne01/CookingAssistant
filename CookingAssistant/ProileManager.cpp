#include "ProfileManager.h"
#include <fstream>
#include <string>
void ProfileManager::save_user_to_file(User user) {
	std::ofstream profile;
	profile.open("profile.txt");
	if (profile.is_open()) {
		profile << user.name << "\n";
		profile << user.age << "\n";
		profile << parse_gender(user.gender) << "\n";
		profile << user.weight << "\n";
		profile << user.height << "\n";
		profile << parse_target(user.target) << "\n";
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
		user.gender = stog(gender);
		std::string weight;
		std::getline(profile, weight);
		user.weight = stod(weight);
		std::string height;
		std::getline(profile, height);
		user.height = stod(height);
		std::string target;
		std::getline(profile, target);
		user.target = stot(target);
		std::cout << "Profile loaded successfully!\n";
	}
	else std::cout << "Failed to open profile!\n";
	profile.close();
}

std::string ProfileManager::parse_gender(Gender gender) {
	if (gender == Gender::male) return "male";
	else if (gender == Gender::female) return "female";
	else return "-";
}

std::string ProfileManager::parse_target(Target target) {
	if (target == Target::weight_loss) return "weight loss";
	else if (target == Target::maintenance) return "weight maintenance";
	else if (target == Target::weight_gain) return "weight gain";
	else return "-";
}

Gender ProfileManager::stog(std::string gender) {
	if (gender == "male") return Gender::male;
	if (gender == "female") return Gender::female;
	else return Gender::not_chosen;
}

Target ProfileManager::stot(std::string target) {
	if (target == "weight loss") return Target::weight_loss;
	if (target == "weight maintenance") return Target::maintenance;
	if (target == "weight gain") return Target::weight_gain;
	else return Target::not_chosen;
}
