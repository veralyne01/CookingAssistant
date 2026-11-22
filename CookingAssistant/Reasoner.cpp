#include "Reasoner.h"
#include "dishes.h"
#include "picker.h"
#include "Auxiliary.h"
#include <math.h>

using namespace Auxiliary;
double Reasoner::meal_type_compliance(const json& f, Dish dish) {
	json options;
	switch (dish.category) {
		case Meal::breakfast:{
			options = f.at("q9_breakfast").at("options");
			break;
		}
		case Meal::lunch: {
			options = f.at("q10_lunch").at("options");
			break;
		}
		case Meal::dinner: {
			options = f.at("q11_dinner").at("options");
			break;
		}
		case Meal::snack: {
			options = f.at("q12_snack").at("options");
			break;
		}
	}
	double result = 0;
	for (const auto& option : options) {
		std::string name = option.at("name").get<std::string>();
		for (const auto& ingredient : dish.ingredients) {
			if (ingredient.name == name) {
				double percentage = option.at("percent").get<double>() / 100;
				result += percentage;
			}
		}
	}
	return result;
}
double Reasoner::user_preferences_compliance(User& user, Dish dish) {
	int likes = 0, dislikes = 0;
	for (const auto& product : user.liked_products) {
		for (const auto& ingredient : dish.ingredients) {
			if (contains_ci(ingredient.name, product)) likes++;
		}
	}
	for (const auto& product : user.disliked_products) {
		for (const auto& ingredient : dish.ingredients) {
			if (contains_ci(ingredient.name, product)) dislikes++;
		}
	}
	if ( likes + dislikes != 0) return static_cast<double>(likes - dislikes)/(likes + dislikes);
	return 0;
}

double Reasoner::calorage_compliance(User& user, Dish dish) {
	int target = 0;
	switch (dish.category) {
		case (Meal::breakfast): {
			target = user.calorie_norm / 100 * 30;
			break;
		}
		case (Meal::lunch): {
			target = user.calorie_norm / 100 * 40;
			break;
		}
		case (Meal::dinner): {
			target = user.calorie_norm / 100 * 20;
			break;
		}
		case (Meal::snack): {
			target = user.calorie_norm / 100 * 10;
			break;
		}
		default: break;
	}
	const double MAX_DEVIATION = target * 0.2;
	return 1 - abs(dish.total_nutrition.calories - target) / MAX_DEVIATION;
}

double Reasoner::cuisine_compliance(const json& f, Dish dish) {
	json cuisines = f.at("q5_cuisine").at("options");
	for (const auto& c : cuisines) {
		if (c.at("name") == dish.cuisine) {
			return c.at("percent") / 100;
		}
	}
	return 0;
}

double Reasoner::total_compliance(const json& f, Dish dish, User& user) {
	double P1 = meal_type_compliance(f, dish);
	double P2 = user_preferences_compliance(user, dish);
	double P3 = calorage_compliance(user, dish);
	double P4 = cuisine_compliance(f, dish);
	return 0.15 * P1 + 0.4 * P2 + 0.35 * P3 + 0.1 * P4;
}