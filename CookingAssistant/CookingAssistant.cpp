#include <iostream>
#include <string.h>
#include <vector>
#include <sql.h>
#include <sqlext.h>
using namespace std;
class Ingredient {
	string name;
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
	string name;
	vector <Ingredient> ingredients;
	enum class DishType {
		breakfast,
		lunch,
		dinner,
		snack
	};
	DishType type;
	int cooking_time_min;
};

class User {
	vector<string> allergens;
	vector<string> disliked_products;
	vector<string> liked_products;
	double weight;
	double height;
	enum class Target {
		weight_loss,
		maintenance,
		weight_gain
	};
	Target target;
};