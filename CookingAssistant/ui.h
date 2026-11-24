#pragma once
#include "user.h"
#include <unordered_set>
#include "picker.h"
#include <nlohmann/json.hpp>

namespace UI{
	void fill_gender(User& user);
	void fill_name(User& user);
	void fill_target(User& user);
	void fill_weight(User& user);
	void fill_height(User& user);
	void fill_activity_level(User& user);
	void ask_calorie_norm(User& user);
	void fill_calorie_norm(User& user, int cal);
	void menu(User& user);
    int read_menu_option(int min_value, int max_value);
	void profile_menu(User& user);
	void fill_age(User& user);
	void input_profile(User& user);
	void edit_profile(User& user);
	void profile_options(User& user);
	void choose_allergies(User& user);
	void add_allergies_from_json(User& user, const std::string category, nlohmann::json& j);
	std::string choose_meal_category();
	void show_result(User& user, std::string category);
};
