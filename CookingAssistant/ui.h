#pragma once
#include "user.h"
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
	void profile_menu(User& user);
	void fill_age(User& user);
	void input_profile(User& user);
	void edit_profile(User& user);
	void profile_options(User& user);
	void choose_allergies(User& user);
	std::string choose_meal_category();
};
