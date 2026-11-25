
#include "Reasoner.h"
#include "dishes.h"
#include "picker.h"
#include "Auxiliary.h"
#include "enums.h"

#include <cmath>
#include <algorithm>
#include <fstream>
#include <string>

using namespace Auxiliary;

double Reasoner::meal_type_compliance(
    const json& j,
    const Dish& dish,
    const std::string& meal_type
) {
    if (meal_type != "breakfast" &&
        meal_type != "lunch" &&
        meal_type != "dinner" &&
        meal_type != "snack") {
        return 0.0;
    }

    const json& options = j.at("groups");

    double result = 0.0;
    double total_weight = 0.0;

    for (const auto& option : options) {
        const auto& weights = option.at("weights");

        auto weight_it = weights.find(meal_type);

        if (weight_it == weights.end()) {
            continue;
        }

        double weight = weight_it->get<double>();
        total_weight += weight;

        bool group_matches = false;

        for (const auto& ingredient : dish.ingredients) {
            for (const auto& keyword : option.at("keywords")) {
                std::string word = keyword.get<std::string>();

                if (contains_ci(ingredient.name, word)) {
                    group_matches = true;
                    break;
                }
            }

            if (group_matches) {
                break;
            }
        }

        if (group_matches) {
            result += weight;
        }
    }

    if (total_weight == 0.0) {
        return 0.0;
    }

    return result / total_weight;
}

double Reasoner::user_preferences_compliance(
    const User& user,
    const Dish& dish
) {
    int likes = 0;
    int dislikes = 0;

    for (const auto& product : user.liked_products) {
        for (const auto& ingredient : dish.ingredients) {
            if (contains_ci(ingredient.name, product)) {
                likes++;
            }
        }
    }

    for (const auto& product : user.disliked_products) {
        for (const auto& ingredient : dish.ingredients) {
            if (contains_ci(ingredient.name, product)) {
                dislikes++;
            }
        }
    }

    if (likes + dislikes == 0) {
        return 0.0;
    }

    return static_cast<double>(likes - dislikes) /
        (likes + dislikes);
}

double Reasoner::calorage_compliance(
    const User& user,
    const Dish& dish
) {
    double meal_fraction = 0.0;

    if (dish.category == "breakfast") {
        meal_fraction = 0.30;
    }
    else if (dish.category == "lunch") {
        meal_fraction = 0.40;
    }
    else if (dish.category == "dinner") {
        meal_fraction = 0.20;
    }
    else if (dish.category == "snack") {
        meal_fraction = 0.10;
    }

    const double target = user.calorie_norm * meal_fraction;

    if (target <= 0.0) {
        return 0.0;
    }

    const double deviation =
        std::abs(dish.total_nutrition.calories - target);

    return std::max(
        0.0,
        1.0 - deviation / (target * 0.2)
    );
}

double Reasoner::cuisine_compliance(
    const json& f,
    const Dish& dish
) {
    if (!f.contains("questions") ||
        !f.at("questions").contains("q5_cuisine") ||
        !f.at("questions").at("q5_cuisine").contains("options")) {
        return 0.0;
    }
    const json& cuisines =
        f.at("questions").at("q5_cuisine").at("options");

    auto it = cuisines.find(dish.cuisine);

    if (it == cuisines.end()) {
        return 0.0;
    }

    return it.value().value("percent", 0.0) / 100.0;
}

double Reasoner::total_compliance(
    const Dish& dish,
    const User& user,
    const std::string& meal_type
) {
    static const json survey = [] {
        std::ifstream survey_file("survey_results.json");
        if (!survey_file.is_open()) {
            return json::object();
        }
        try {
            return json::parse(survey_file);
        } catch (const json::exception&) {
            return json::object();
        }
    }();
    static const json rules = load_json("meal_rules.json");

    const double P1 = meal_type_compliance(
        rules, dish, meal_type
    );

    const double P2 = user_preferences_compliance(
        user, dish
    );

    const double P3 = calorage_compliance(
        user, dish
    );

    const double P4 = cuisine_compliance(
        survey, dish
    );

    return 0.15 * P1 +
        0.40 * P2 +
        0.35 * P3 +
        0.10 * P4;
}
