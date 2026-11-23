#include <vector>
#include "enums.h"
#include <string>
#include "user.h"
#include "enums.h"
double User::count_appr_calories(User user) {
	double res = 10 * user.weight + 6.25 * user.height * 100 - 5 * user.age + 5;
	if (user.gender == "female") return (res - 166) * choose_coef(user.activity_level);
	return res * choose_coef(user.activity_level);
}
double User::choose_coef(const std::string& level) {
    auto it = Values::activity_coefficients.find(level);
    return it == Values::activity_coefficients.end() ? 0.0 : it->second;
}
//целевой калораж 
double User::count_target_calories(User user) {
	double prop_calories = count_appr_calories(user);
	if (user.target == "weight_loss") return 0.85 * prop_calories;
	else if (user.target == "weight_gain") return 1.15 * prop_calories;
	else return prop_calories;
}
