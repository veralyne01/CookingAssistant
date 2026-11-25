#include "sqlmanager.h"
#include "nutrition_calculator.h"

#include <sqlite3.h>

namespace SQLManager {
	void fill_nutrients(std::vector<Ingredient>& ingredients) {
		sqlite3* db = nullptr;
		int result = sqlite3_open_v2("data/cookingassistant_foods.sqlite", &db,
			SQLITE_OPEN_READONLY, nullptr);
		if (result != SQLITE_OK) {
			if (db != nullptr) {
				sqlite3_close(db);
			}
			return;
		}
		for(auto& ingredient : ingredients) {
			sqlite3_stmt* statement = nullptr;

			const char* sql =
				"SELECT i.* "
				"FROM ingredients i "
				"LEFT JOIN aliases a ON a.ingredient_id = i.id "
				"WHERE LOWER(i.name) = LOWER(?) "
				"OR LOWER(a.alias) = LOWER(?) "
				"LIMIT 1;";

			if (sqlite3_prepare_v2(db, sql, -1, &statement, nullptr) != SQLITE_OK ||
				statement == nullptr) {
				if (statement != nullptr) {
					sqlite3_finalize(statement);
				}
				continue;
			}
			sqlite3_bind_text(statement, 1, ingredient.name.c_str(), -1, SQLITE_TRANSIENT);
			sqlite3_bind_text(statement, 2, ingredient.name.c_str(), -1, SQLITE_TRANSIENT);
			int output = sqlite3_step(statement);
			if (output == SQLITE_ROW)
			{
				double calories =
					sqlite3_column_double(statement, 3);

				double proteins =
					sqlite3_column_double(statement, 4);

				double fats =
					sqlite3_column_double(statement, 5);

				double carbohydrates =
					sqlite3_column_double(statement, 6);

				ingredient.nutrition_per_100g.calories = calories;
				ingredient.nutrition_per_100g.proteins = proteins;
				ingredient.nutrition_per_100g.fats = fats;
				ingredient.nutrition_per_100g.carbohydrates = carbohydrates;
				if (ingredient.weight_grams.has_value()) {
					ingredient.nutrition_for_amount = NutritionCalculator::calculate_ingredient(*ingredient.weight_grams, ingredient.nutrition_per_100g);
					ingredient.nutrition_calculated = true;
				}
			}
			sqlite3_finalize(statement);
		}
		sqlite3_close(db);
	}
}
