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

class Recipe {
public:
	std::string name;
	std::vector<Ingredient> ingredients;
	Meal meal;
	int cooking_time_min;
};

struct Dish {
	std::string id;                       // idMeal из TheMealDB (уникальный ключ)
	std::string name;
	std::string category;
	std::string area;
	std::string instructions;
	std::vector<std::string> ingredients; // только непустые, без пар из 20 полей
	std::vector<std::string> measures;    // measures[i] соответствует ingredients[i]
};