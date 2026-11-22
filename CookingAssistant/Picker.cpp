#include "picker.h"
#include "mealdbsearch.h"
#include <algorithm>
#include <cctype>
#include <chrono>
#include <random>
#include <thread>

bool Picker::is_allowed(const Dish& dish, const std::vector<std::string>& banned) {
    if (dish.ingredients.empty()) return false;
    for (const auto& ingredient : dish.ingredients) {
        for (const auto& word : banned) {
            if (contains_ci(ingredient.name, word)) return false;
        }
    }
    return true;
}

double Picker::score(const Dish& dish, const User& user) {
    double result = 0.0;
    for (const auto& allergen : user.allergens) {
        for (const auto& ingredient : dish.ingredients) {
            if (contains_ci(ingredient.name, allergen)) {
                return result;
            }
        }
    }
    for (const auto& liked : user.liked_products) {
        for (const auto& ingredient : dish.ingredients) {
            if (contains_ci(ingredient.name, liked)) {
                result += 1.0;
                break;
            }
        }
    }
    for (const auto& disliked : user.disliked_products) {
        for (const auto& ingredient : dish.ingredients) {
            if (contains_ci(ingredient.name, disliked)) {
                result -= 1.0;
                break;
            }
        }
    }
    return result;
}

std::vector<Dish> Picker::find_candidates(const User& user,
    const std::unordered_set<std::string>& already_shown, std::string category) {

    const std::vector<std::string> banned = banned_list(user);
    std::vector<Dish> result;
    std::unordered_set<std::string> seen = already_shown;

    // Проверяет блюдо один раз и добавляет, если оно подходит
    auto consider = [&](const Dish& dish) {
        if (dish.id.empty() || !seen.insert(dish.id).second) return;
        if (is_allowed(dish, banned)) result.push_back(dish);
        };

    // Источник 1: категория Breakfast (приходят только id, ингредиенты запрашиваем отдельно)
    const std::vector<std::string> ids = MealDB::filter_ids_by_category(category);
    std::size_t lookups = 0;
    for (const auto& id : ids) {
        if (result.size() >= MIN_CANDIDATES || lookups >= MAX_LOOKUPS) break;
        if (seen.count(id)) continue;
        std::this_thread::sleep_for(PAUSE);
        ++lookups;
        if (auto dish = MealDB::lookup_by_id(id)) consider(*dish);
    }
    std::vector<std::string> KEYWORDS;
    if (category == "Breakfast") KEYWORDS = BREAKFAST_KEYWORDS;
    else if (category == "Lunch") KEYWORDS = LUNCH_KEYWORDS;
    else if (category == "Dinner") KEYWORDS = DINNER_KEYWORDS;
    else KEYWORDS = SNACK_KEYWORDS;

    // Источник 2: поиск по ключевым словам (в ответе сразу полные данные)
    for (const auto& word : KEYWORDS) {
        if (result.size() >= MIN_CANDIDATES) break;
        std::this_thread::sleep_for(PAUSE);
        for (const auto& dish : MealDB::search_by_name(word)) consider(dish);
    }

    return result;
}

std::optional<Dish> Picker::suggest(const User& user,
    std::unordered_set<std::string>& already_shown, std::string category) {

    std::vector<Dish> candidates = find_candidates(user, already_shown, category);
    if (candidates.empty()) return std::nullopt;

    // Лучшие по оценке вперёд
    std::stable_sort(candidates.begin(), candidates.end(),
        [&](const Dish& a, const Dish& b) { return score(a, user) > score(b, user); });

    // Случайный выбор среди лучших, чтобы не предлагать одно и то же каждый раз
    static std::mt19937 rng{ std::random_device{}() };
    const std::size_t top = std::min<std::size_t>(candidates.size(), TOP_N);
    std::uniform_int_distribution<std::size_t> pick(0, top - 1);
    const Dish chosen = candidates[pick(rng)];

    already_shown.insert(chosen.id);
    return chosen;
}