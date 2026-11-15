#pragma once
#include "user.h"
class ProfileManager
{
public:
	static void save_user_to_file(User user);
	static void read_user_from_file(User& user);
};

