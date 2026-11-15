#pragma once
#include <iostream>
#include <vector>
#include "enums.h"

class Ingredient {
	std::string name;
	double calories;
	double proteins;
	double fats;
	double carbohydrates;
	Categories category;
};

struct Dish {
	std::string id;                     
	std::string name;
	std::string category;
	std::string area;
	std::string instructions;
	std::vector<std::string> ingredients; 
	std::vector<std::string> measures;   
};