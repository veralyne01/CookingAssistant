#pragma once
#include <iostream>
#include <string>
#include <vector>

class User{
public:
	std::string name;
	std::vector<std::string> allergens;
	std::vector<std::string> disliked_products;
	std::vector<std::string> liked_products;
	double weight = 0;
	double height = 0;
	int age = 0;
	std::string target = "not_chosen";
	std::string gender = "not_chosen";
	std::string activity_level = "not_chosen";
	int calorie_norm = 0;

	static double count_appr_calories(User user);
	static double choose_coef(const std::string& level);
	static double count_target_calories(User user);
};