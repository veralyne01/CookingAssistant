#include <vector>
#include "enums.h"
#include <string>
#include "user.h"
#include <fstream>
#include <iostream>
double User::count_appr_calories(User user) {
	double res = 10 * user.weight + 6.25 * user.height * 100 - 5 * user.age + 5;
	if (user.gender == Gender::female) return (res - 166) * choose_coef(user.activity_level);
	return res * choose_coef(user.activity_level);
}
double User::choose_coef(Activity_level level) {
	switch (level) {
	case Activity_level::no_activity: return 1.2;
	case Activity_level::light_exertion: return 1.375;
	case Activity_level::regular_workouts: return 1.55;
	case Activity_level::frequent_workouts: return 1.725;
	case Activity_level::hard_work: return 1.9;
	default: return 0;
	}
}
//целевой калораж 
double User::count_target_calories(User user) {
	double prop_calories = count_appr_calories(user);
	if (user.target == Target::weight_loss) return 0.85 * prop_calories;
	else if (user.target == Target::weight_gain) return 1.15 * prop_calories;
	else return prop_calories;
}
