#include <string>
#include <unordered_map>
#include "enums.h"
#include "parsers.h"
#include <algorithm>
#include <sstream>

std::string Parser::parse_gender(Gender gender) {
	if (gender == Gender::male) return "male";
	else if (gender == Gender::female) return "female";
	else return "-";
}

std::string Parser::parse_target(Target target) {
	if (target == Target::weight_loss) return "weight loss";
	else if (target == Target::maintenance) return "weight maintenance";
	else if (target == Target::weight_gain) return "weight gain";
	else return "-";
}

std::string Parser::parse_activity_level(Activity_level activity_level) {
	if (activity_level == Activity_level::no_activity) return "physical activity is absent or minimal";
	if (activity_level == Activity_level::light_exertion) return "moderate-intensity workouts 1-2 times a week";
	if (activity_level == Activity_level::frequent_workouts) return "intensive workout every day or physical labor";
	if (activity_level == Activity_level::regular_workouts) return "moderate-intensity workouts 3-5 times a week";
	if (activity_level == Activity_level::hard_work) return "intensive workout every day and physically demanding job";
	else return "-";
}

std::string Parser::parse_meal(Meal meal) {
    if (meal == Meal::breakfast) return "breakfast";
    if (meal == Meal::lunch) return "lunch";
    if (meal == Meal::dinner) return "dinner";
    if (meal == Meal::snack) return "snack";
    else return "-";
}

Gender Parser::stog(std::string gender) {
	if (gender == "male") return Gender::male;
	if (gender == "female") return Gender::female;
	else return Gender::not_chosen;
}

Target Parser::stot(std::string target) {
	if (target == "weight loss") return Target::weight_loss;
	if (target == "weight maintenance") return Target::maintenance;
	if (target == "weight gain") return Target::weight_gain;
	else return Target::not_chosen;
}

Activity_level Parser::stoa(std::string activity_level) {
	if (activity_level == "physical activity is absent or minimal") return Activity_level::no_activity;
	if (activity_level == "moderate-intensity workouts 1-2 times a week") return Activity_level::light_exertion;
	if (activity_level == "moderate-intensity workouts 3-5 times a week") return Activity_level::regular_workouts;
	if (activity_level == "intensive workout every day or physical labor") return Activity_level::frequent_workouts;
	if (activity_level == "intensive workout every day and physically demanding job") return Activity_level::hard_work;
	return Activity_level::not_chosen;
}

template <typename T>
T str_to_val(const std::string& key,
    const std::unordered_map<std::string, T>& map) {
    auto it = map.find(key);
    if (it != map.end())
        return it->second;
    throw std::runtime_error("Key not found!");
}

template <typename T>
std::string val_to_str(const T& value,
    const std::unordered_map<std::string, T>& map) {
    for (const auto& [key, val] : map) {
        if (val == value)
            return key;
    }
    throw std::runtime_error("Value not found!");
}

// Разбирает строку вроде "1 3, 5" в список номеров вариантов.
// 0 означает "ничего не выбрано" и допустим только сам по себе.
// Возвращает false и заполняет error, если ввод некорректен.
bool parse_choices(std::string line, int max_option,
    std::vector<int>& result, std::string& error) {
    result.clear();
    std::replace(line.begin(), line.end(), ',', ' ');   // запятые считаем пробелами

    std::istringstream stream(line);
    std::string token;
    bool has_zero = false;

    while (stream >> token) {
        int number = 0;
        try {
            std::size_t pos = 0;
            number = std::stoi(token, &pos);
            if (pos != token.size()) {                  // например, "3a"
                error = "'" + token + "' is not a number";
                return false;
            }
        }
        catch (const std::exception&) {                 // "abc" или слишком большое
            error = "'" + token + "' is not a number";
            return false;
        }

        if (number < 0 || number > max_option) {
            error = "Option " + std::to_string(number) + " does not exist";
            return false;
        }

        if (number == 0) {
            has_zero = true;
        }
        else if (std::find(result.begin(), result.end(), number) == result.end()) {
            result.push_back(number);                   // повторы схлопываем
        }
    }

    if (has_zero && !result.empty()) {
        error = "0 cannot be combined with other options";
        return false;
    }
    if (!has_zero && result.empty()) {
        error = "Please choose at least one option (or 0)";
        return false;
    }
    return true;   // пустой result при has_zero означает "нет аллергий"
}