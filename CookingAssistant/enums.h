#pragma once
#include <iostream>
#include <unordered_map>
#include <string>
enum class Gender {
	male,
	female,
	not_chosen
};
enum class Target {
	weight_loss,
	maintenance,
	weight_gain,
	not_chosen
};
enum class Activity_level {
	no_activity,
	light_exertion,
	regular_workouts,
	frequent_workouts,
	hard_work,
	not_chosen
};
enum class Meal {
	breakfast,
	lunch,
	dinner,
	snack
};

enum class Categories {
	vegetable,
	fruit,
	dairy,
	cereal,
	meat,
	berry,
	other
};
