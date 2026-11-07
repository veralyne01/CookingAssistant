#pragma once
#include "user.h"
class UI{
public:
	static void fill_gender(User& user);
	static void fill_name(User& user);
	static void fill_target(User& user);
	static void fill_weight(User& user);
	static void fill_height(User& user);
	static void menu(User& user);
	static void profile_menu(User& user);
	static void fill_age(User& user);
	static void input_profile(User& user);
	static void edit_profile(User& user);
	static void profile_options(User& user);
};