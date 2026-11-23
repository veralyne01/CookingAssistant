# Cooking Assistant

Console C++20 application for recipe suggestions and nutritional calculations.

## Refactored structure
- Domain choices are string keys backed by dictionaries in `enums.h` (`Values`).
- User profile is stored in `profile.json`.
- Meal selection accepts numeric input through a reusable range validator.
- TheMealDB searches use the Breakfast category and keyword searches for lunch, dinner and snacks; candidate lists are capped at 20 and de-duplicated.
- USDA nutrition parsing accepts numeric JSON values and numeric strings; invalid/non-numeric nutrient values are skipped.

## API limitation
TheMealDB does not provide native Lunch, Dinner or Snack categories in the same way it provides Breakfast. Those options are therefore approximated by recipe-name keyword searches. The application may return fewer than 20 dishes if the API does not yield enough unique recipes or the allergen filter removes them.

## Build
Open `CookingAssistant.vcxproj` in Visual Studio. The project uses WinHTTP and nlohmann/json. API key for USDA is read from the `FDC_API_KEY` environment variable.
