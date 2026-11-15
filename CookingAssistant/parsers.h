#pragma once
#include <string>
#include "enums.h"
class Parser {
public:
	static std::string parse_gender(Gender gender);
	static std::string parse_target(Target target);
	static std::string parse_meal(Meal meal);
	static std::string parse_activity_level(Activity_level activity_level);
	static Gender stog(std::string gender);
	static Target stot(std::string target);
	static Activity_level stoa(std::string activity_level);
};