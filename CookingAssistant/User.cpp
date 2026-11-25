#include "user.h"

#include "enums.h"

double User::count_appr_calories(const User& user) {
  double result = 10.0 * user.weight + 6.25 * user.height * 100.0 -
                  5.0 * user.age + 5.0;
  if (user.gender == "female") {
    result -= 166.0;
  }
  return result * choose_coef(user.activity_level);
}

double User::choose_coef(const std::string& level) {
  const auto iterator = Values::activity_coefficients.find(level);
  return iterator == Values::activity_coefficients.end() ? 0.0
                                                         : iterator->second;
}

double User::count_target_calories(const User& user) {
  const double calories = count_appr_calories(user);
  if (user.target == "weight_loss") {
    return 0.85 * calories;
  }
  if (user.target == "weight_gain") {
    return 1.15 * calories;
  }
  return calories;
}
