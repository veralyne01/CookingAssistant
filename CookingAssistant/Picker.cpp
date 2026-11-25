#include "picker.h"

#include <algorithm>
#include <random>
#include <unordered_map>
#include <utility>

#include "Auxiliary.h"
#include "meal_db_search.h"
#include "food_data_central.h"
#include "Reasoner.h"
#include "recipe_storage.h"
#include "user.h"

namespace Picker {

const std::vector<std::string> BREAKFAST_KEYWORDS = {
    "pancake", "omelette", "porridge", "waffle", "toast", "oatmeal",
    "crepe", "granola", "cottage"};
const std::vector<std::string> LUNCH_KEYWORDS = {
    "soup", "fish", "chicken", "beef", "pasta", "salad", "rice", "curry"};
const std::vector<std::string> DINNER_KEYWORDS = {
    "chicken", "beef", "fish", "pasta", "curry", "stew", "roast",
    "casserole"};
const std::vector<std::string> SNACK_KEYWORDS = {
    "sandwich", "salad", "toast", "smoothie", "fruit", "muffin", "snack",
    "cake"};

namespace {

bool is_disliked(const Dish& dish, const User& user) {
  for (const std::string& disliked : user.disliked_dishes) {
    if (Auxiliary::contains_ci(dish.id, disliked) ||
        Auxiliary::contains_ci(dish.name, disliked)) {
      return true;
    }
  }
  return false;
}

bool is_used(const Dish& dish,
             const std::unordered_set<std::string>& used_ids) {
  return !dish.id.empty() && used_ids.count(dish.id) > 0;
}

std::optional<Dish> choose_best(const std::vector<Dish>& dishes,
                                const User& user,
                                const std::string& category) {
  if (dishes.empty()) {
    return std::nullopt;
  }

  std::vector<ScoredDish> scored = sort_candidates(
      get_scored_candidates(dishes, user, category));
  if (scored.empty()) {
    return std::nullopt;
  }

  const std::size_t pool_size = std::min<std::size_t>(3, scored.size());
  static std::mt19937 generator(std::random_device{}());
  std::uniform_int_distribution<std::size_t> distribution(0, pool_size - 1);
  return scored[distribution(generator)].dish;
}

}

std::vector<Dish> remove_allergens(const std::vector<Dish>& dishes,
                                   const User& user) {
  std::vector<Dish> result;
  for (const Dish& dish : dishes) {
    bool excluded = is_disliked(dish, user);
    for (const Ingredient& ingredient : dish.ingredients) {
      for (const std::string& allergen : user.allergens) {
        if (Auxiliary::contains_ci(ingredient.name, allergen)) {
          excluded = true;
          break;
        }
      }
      if (excluded) {
        break;
      }
    }
    if (!excluded) {
      result.push_back(dish);
    }
  }
  return result;
}

std::vector<Dish> find_candidates(
    const User& user, const std::string& category, bool include_local,
    const std::unordered_set<std::string>& used_ids) {
  std::vector<Dish> result;
  std::unordered_set<std::string> seen;
  const std::vector<Dish> liked_recipes = RecipeStorage::load_liked_recipes();
  const std::unordered_set<std::string> seen_ids =
      RecipeStorage::load_seen_recipe_ids();
  std::unordered_set<std::string> liked_ids;
  for (const Dish& saved : liked_recipes) {
    if (!saved.id.empty()) {
      liked_ids.insert(saved.id);
    }
  }

  if (include_local) {
    for (const Dish& saved : liked_recipes) {
      if (saved.category == category && !is_used(saved, used_ids) &&
          !is_disliked(saved, user) &&
          (saved.id.empty() || seen.insert(saved.id).second)) {
        result.push_back(saved);
      }
    }
  }

  const std::unordered_map<std::string, std::vector<std::string>> search_terms = {
      {"breakfast", BREAKFAST_KEYWORDS}, {"lunch", LUNCH_KEYWORDS},
      {"dinner", DINNER_KEYWORDS}, {"snack", SNACK_KEYWORDS}};
  const auto terms = search_terms.find(category);

  if (terms != search_terms.end()) {
    for (const std::string& term : terms->second) {
      for (Dish& dish : MealDB::search_by_name(term)) {
        dish.category = category;
        if (is_used(dish, used_ids) ||
            (!include_local && !dish.id.empty() && liked_ids.count(dish.id) > 0) ||
            (!dish.id.empty() && !seen.insert(dish.id).second) ||
            seen_ids.count(dish.id) > 0) {
          continue;
        }
        result.push_back(std::move(dish));
      }
    }
  }

  return remove_allergens(result, user);
}

std::vector<ScoredDish> get_scored_candidates(
    const std::vector<Dish>& dishes, const User& user,
    const std::string& category) {
  std::vector<ScoredDish> scored;
  scored.reserve(dishes.size());
  for (const Dish& source_dish : dishes) {
    Dish dish = source_dish;
    FoodDataCentral::calculate_dish(dish);
    const double score = Reasoner::total_compliance(dish, user, category);
    scored.push_back({std::move(dish), score});
  }
  return scored;
}

std::vector<ScoredDish> sort_candidates(std::vector<ScoredDish> dishes) {
  std::stable_sort(dishes.begin(), dishes.end(),
                   [](const ScoredDish& first, const ScoredDish& second) {
                     return first.parameter > second.parameter;
                   });
  return dishes;
}

std::optional<Dish> find_suitable_dish(
    User& user, const std::string& category, bool include_local,
    const std::unordered_set<std::string>& used_ids) {
  const std::vector<Dish> candidates =
      find_candidates(user, category, include_local, used_ids);
  return choose_best(candidates, user, category);
}

std::vector<std::optional<Dish>> suggest_daily_menu(
    User& user, std::unordered_set<std::string>* used_ids) {
  std::unordered_set<std::string> local_used;
  if (used_ids == nullptr) {
    local_used = RecipeStorage::load_menu_used_ids();
    const std::vector<Dish> saved_recipes = RecipeStorage::load_liked_recipes();
    std::size_t used_local_count = 0;
    for (const Dish& dish : saved_recipes) {
      if (!dish.id.empty() && local_used.count(dish.id) > 0) {
        ++used_local_count;
      }
    }
    if (!saved_recipes.empty() && used_local_count >= saved_recipes.size()) {
      RecipeStorage::reset_menu_used_ids();
      local_used.clear();
    }
    used_ids = &local_used;
  }

  const std::vector<std::string> categories = {
      "breakfast", "lunch", "dinner", "snack"};
  const std::vector<Dish> liked_recipes = RecipeStorage::load_liked_recipes();
  std::unordered_set<std::string> local_ids;
  for (const Dish& dish : liked_recipes) {
    if (!dish.id.empty()) {
      local_ids.insert(dish.id);
    }
  }
  std::vector<std::optional<Dish>> menu;
  menu.reserve(categories.size());

  bool has_new_dish = false;
  for (const std::string& category : categories) {
    std::vector<Dish> candidates =
        find_candidates(user, category, true, *used_ids);
    std::vector<Dish> local_candidates;
    std::vector<Dish> new_candidates;

    for (Dish& dish : candidates) {
      const bool is_local = !dish.id.empty() && local_ids.count(dish.id) > 0;
      if (is_local) {
        local_candidates.push_back(std::move(dish));
      } else {
        new_candidates.push_back(std::move(dish));
      }
    }

    std::optional<Dish> selected =
        choose_best(local_candidates, user, category);
    if (!selected.has_value()) {
      selected = choose_best(new_candidates, user, category);
    }

    if (selected.has_value()) {
      if (!selected->id.empty()) {
        used_ids->insert(selected->id);
      }
      const bool is_local = !selected->id.empty() &&
          local_ids.count(selected->id) > 0;
      if (!is_local) {
        has_new_dish = true;
      } else if (used_ids == &local_used) {
        RecipeStorage::mark_menu_recipe_used(selected->id);
      }
    }
    menu.push_back(std::move(selected));
  }

  if (!has_new_dish) {
    for (std::size_t index = 0; index < menu.size(); ++index) {
      if (menu[index].has_value()) {
        const std::string category = categories[index];
        std::optional<Dish> fresh = find_suitable_dish(
            user, category, false, *used_ids);
        if (fresh.has_value()) {
          if (!menu[index]->id.empty()) {
            used_ids->erase(menu[index]->id);
          }
          menu[index] = std::move(fresh);
          if (!menu[index]->id.empty()) {
            used_ids->insert(menu[index]->id);
          }
          break;
        }
      }
    }
  }

  return menu;
}

}
