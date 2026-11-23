#pragma once
#include <string>
#include <unordered_map>

namespace Values {
inline const std::unordered_map<std::string, std::string> genders = {{"male", "male"}, {"female", "female"}, {"not_chosen", "-"}};
inline const std::unordered_map<std::string, std::string> targets = {{"weight_loss", "weight loss"}, {"maintenance", "weight maintenance"}, {"weight_gain", "weight gain"}, {"not_chosen", "-"}};
inline const std::unordered_map<std::string, std::string> activity_levels = {{"no_activity", "physical activity is absent or minimal"}, {"light_exertion", "moderate-intensity workouts 1-2 times a week"}, {"regular_workouts", "moderate-intensity workouts 3-5 times a week"}, {"frequent_workouts", "intensive workout every day or physical labor"}, {"hard_work", "intensive workout every day and physically demanding job"}, {"not_chosen", "-"}};
inline const std::unordered_map<std::string, double> activity_coefficients = {{"no_activity", 1.2}, {"light_exertion", 1.375}, {"regular_workouts", 1.55}, {"frequent_workouts", 1.725}, {"hard_work", 1.9}};
inline const std::unordered_map<std::string, std::string> meals = {{"breakfast", "Breakfast"}, {"lunch", "Lunch"}, {"dinner", "Dinner"}, {"snack", "Snack"}, {"not_defined", "-"}};
inline const std::unordered_map<std::string, std::string> meal_categories = {{"breakfast", "Breakfast"}, {"lunch", "Lunch"}, {"dinner", "Dinner"}, {"snack", "Snack"}};
}
