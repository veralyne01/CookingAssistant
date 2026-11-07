#pragma once
#include "user.h"
class ProfileManager
{
public:
	static void save_user_to_file(User user);
	static void read_user_from_file(User& user);
	static std::string parse_gender(Gender gender);
	static std::string parse_target(Target target);
	static std::string parse_activity_level(Activity_level activity_level);
	static Gender stog(std::string gender);
	static Target stot(std::string target);
	static Activity_level stoa(std::string target);
};

