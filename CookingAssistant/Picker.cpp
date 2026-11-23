#include "picker.h"
#include "meal_db_search.h"
#include "auxiliary.h"
#include <algorithm>
#include <cctype>
#include <chrono>
#include <random>
#include <thread>
#include <unordered_map>
#include <utility>
#include <utility>

using namespace Auxiliary;

namespace {
constexpr std::size_t kTopCandidates = 5;
std::vector<std::string> banned_list(const User& user) {
    std::vector<std::string> banned = user.allergens;
    banned.insert(banned.end(), user.disliked_products.begin(), user.disliked_products.end());
    return banned;
}
}

bool Picker::is_allowed(const Dish& dish, const std::vector<std::string>& banned) {
    if (dish.ingredients.empty()) return false;
    for (const auto& ingredient : dish.ingredients) {
        for (const auto& word : banned) {
            if (Auxiliary::contains_ci(ingredient.name, word)) return false;
        }
    }
    return true;
}

double Picker::score(const Dish& dish, const User& user) {
    double result = 0.0;
    for (const auto& allergen : user.allergens) {
        for (const auto& ingredient : dish.ingredients) {
            if (Auxiliary::contains_ci(ingredient.name, allergen)) {
                return result;
            }
        }
    }
    for (const auto& liked : user.liked_products) {
        for (const auto& ingredient : dish.ingredients) {
            if (Auxiliary::contains_ci(ingredient.name, liked)) {
                result += 1.0;
                break;
            }
        }
    }
    for (const auto& disliked : user.disliked_products) {
        for (const auto& ingredient : dish.ingredients) {
            if (Auxiliary::contains_ci(ingredient.name, disliked)) {
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
    constexpr std::size_t kMaxCandidates = 20;
    constexpr std::size_t kMaxLookups = 40;

    auto consider = [&](Dish dish) {
        if (dish.id.empty() || !seen.insert(dish.id).second) return;
        dish.category = category;
        if (is_allowed(dish, banned)) result.push_back(std::move(dish));
    };

    // Breakfast is a native TheMealDB category; other meal types use name queries.
    if (category == "breakfast") {
        const auto ids = MealDB::filter_ids_by_category("Breakfast");
        std::size_t lookups = 0;
        for (const auto& id : ids) {
            if (result.size() >= kMaxCandidates || lookups >= kMaxLookups) break;
            if (seen.count(id)) continue;
            std::this_thread::sleep_for(PAUSE);
            ++lookups;
            if (auto dish = MealDB::lookup_by_id(id)) consider(std::move(*dish));
        }
    }


    const std::unordered_map<std::string, std::vector<std::string>> search_terms = {
        {"breakfast", BREAKFAST_KEYWORDS}, {"lunch", LUNCH_KEYWORDS},
        {"dinner", DINNER_KEYWORDS}, {"snack", SNACK_KEYWORDS}
    };
    auto terms = search_terms.find(category);
    if (terms != search_terms.end()) {
        for (const auto& term : terms->second) {
            if (result.size() >= kMaxCandidates) break;
            std::this_thread::sleep_for(PAUSE);
            for (auto dish : MealDB::search_by_name(term)) {
                consider(std::move(dish));
                if (result.size() >= kMaxCandidates) break;
            }
        }
    }
    if (result.size() > kMaxCandidates) result.resize(kMaxCandidates);
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
    const std::size_t top = std::min<std::size_t>(candidates.size(), kTopCandidates);
    std::uniform_int_distribution<std::size_t> pick(0, top - 1);
    const Dish chosen = candidates[pick(rng)];

    already_shown.insert(chosen.id);
    return chosen;
}