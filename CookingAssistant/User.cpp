#include <vector>
#include "enums.h"
#include <string>
#include "user.h"
#include <fstream>
#include <iostream>
double User::count_appr_calories(double weight, double height, int age, Gender gender, Activity_level level) {
	double res = 10 * weight + 6.25 * height - 5 * age + 5;
	if (gender == Gender::female) return (res - 166) * choose_coef(level);
	return res * choose_coef(level);
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
double User::count_target_calories(double prop_calories, Target target, Activity_level level) {
	if (target == Target::weight_loss) return 0.85 * prop_calories;
	else if (target == Target::weight_gain) return 1.15 * prop_calories;
	else return prop_calories;
}
