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
		profile << parse_activity_level(user.activity_level) << "\n";
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
		std::string activity_level;
		std::getline(profile, activity_level);
		user.activity_level = stoa(activity_level);
		std::string cal;
		std::getline(profile, cal);
		user.calorie_norm = stoi(cal);
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

std::string ProfileManager::parse_activity_level(Activity_level activity_level) {
	if (activity_level == Activity_level::no_activity) return "physical activity is absent or minimal";
	if (activity_level == Activity_level::light_exertion) return "moderate-intensity workouts 1-2 times a week";
	if (activity_level == Activity_level::frequent_workouts) return "intensive workout every day or physical labor";
	if (activity_level == Activity_level::regular_workouts) return "moderate-intensity workouts 3-5 times a week";
	if (activity_level == Activity_level::hard_work) return "intensive workout every day and physically demanding job";
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

Activity_level ProfileManager::stoa(std::string activity_level) {
	if (activity_level == "physical activity is absent or minimal") return Activity_level::no_activity;
	if (activity_level == "moderate-intensity workouts 1-2 times a week") return Activity_level::light_exertion;
	if (activity_level == "moderate-intensity workouts 3-5 times a week") return Activity_level::regular_workouts;
	if (activity_level == "intensive workout every day or physical labor") return Activity_level::frequent_workouts;
	if (activity_level == "intensive workout every day and physically demanding job") return Activity_level::hard_work;
	return Activity_level::not_chosen;
}
