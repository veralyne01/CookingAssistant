#pragma once
#include <iostream>
#include <vector>
#include "enums.h"
class User{
public:
	std::string name;
	std::vector<std::string> allergens;
	std::vector<std::string> disliked_products;
	std::vector<std::string> liked_products;
	double weight = 0;
	double height = 0;
	int age = 0;
	Target target = Target::not_chosen;
	Gender gender = Gender::not_chosen;
	Activity_level activity_level = Activity_level::not_chosen;
	int calorie_norm = 0;

	static double count_appr_calories(User user);
	static double choose_coef(Activity_level level);
	static double count_target_calories(User user);
};