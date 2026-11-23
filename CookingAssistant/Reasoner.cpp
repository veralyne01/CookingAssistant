//Функции решения многокритериальной задачи
#include "reasoner.h"
#include "dishes.h"
#include "picker.h"
#include "auxiliary.h"
#include <cmath>
#include <algorithm>
#include <fstream>
#include "enums.h"

using namespace Auxiliary;
double Reasoner::meal_type_compliance(const json& j, Dish dish) {
	json options = j.at("groups");
	std::string meal = dish.category;

	double result = 0.0;
	double total_weight = 0.0;

	for (const auto& option : options) {
		double weight = option.at("weights").at(meal);
		total_weight += weight;

		bool group_matches = false;

		for (const auto& ingredient : dish.ingredients) {
			auto it = std::find(
				option["keywords"].begin(),
				option["keywords"].end(),
				ingredient.name
			);

			if (it != option["keywords"].end()) {
				group_matches = true;
				break;
			}
		}

		if (group_matches) {
			result += weight;
		}
	}

	if (total_weight == 0.0) return 0.0;

	return result / total_weight;
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
    double meal_fraction = 0.0;
    if (dish.category == "breakfast") meal_fraction = 0.30;
    else if (dish.category == "lunch") meal_fraction = 0.40;
    else if (dish.category == "dinner") meal_fraction = 0.20;
    else if (dish.category == "snack") meal_fraction = 0.10;
    const double target = user.calorie_norm * meal_fraction;
    if (target <= 0.0) return 0.0;
    const double deviation = std::abs(dish.total_nutrition.calories - target);
    return std::max(0.0, 1.0 - deviation / (target * 0.2));
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

double Reasoner::total_compliance(Dish dish, User& user) {
	json survey = load_json("survey_results.json");
	json rules = load_json("meal_rules.json");
	double P1 = meal_type_compliance(rules, dish);
	double P2 = user_preferences_compliance(user, dish);
	double P3 = calorage_compliance(user, dish);
	double P4 = cuisine_compliance(survey, dish);
	return 0.15 * P1 + 0.4 * P2 + 0.35 * P3 + 0.1 * P4;
}