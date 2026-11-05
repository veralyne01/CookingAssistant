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

	double count_appr_calories(double weight, double height, int age, Gender gender, Activity_level level);
	double choose_coef(Activity_level level);
	double count_target_calories(double prop_calories, Target target, Activity_level level);
};