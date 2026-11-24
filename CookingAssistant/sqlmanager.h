#pragma once
#include <vector>
#include "dishes.h"
#include <sqlite3.h>

namespace SQLManager {
	static void fill_nutrients(std::vector<Ingredient>& ingredients);
};