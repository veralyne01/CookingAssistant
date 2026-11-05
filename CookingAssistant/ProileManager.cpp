#include "ProfileManager.h"
#include <fstream>
#include <string>
void ProfileManager::save_user_to_file(User user) {
	std::ofstream profile;
	profile.open("profile.txt");
	if (profile.is_open()) {
		profile << user.name;
		profile << user.age;
		profile << std::to_string(user.gender);
	}
}

void ProfileManager::read_user_from_file() {

}
