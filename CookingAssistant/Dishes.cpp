#include <iostream>
#include <vector>
class Ingredient {
	std::string name;
	double calories;
	double proteins;
	double fats;
	double carbohydrates;
	enum class Categories {
		vegetable,
		fruit,
		dairy,
		cereal,
		meat,
		berry,
		other
	};
	Categories category;
};

class Recipe {
	std::string name;
	std::vector<Ingredient> ingredients;
	enum class DishType {
		breakfast,
		lunch,
		dinner,
		snack
	};
	DishType type;
	int cooking_time_min;
};

