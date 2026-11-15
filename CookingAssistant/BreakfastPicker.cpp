#include "BreakfastPicker.h"
#include "mealdbsearch.h"
#include <algorithm>
#include <cctype>
#include <chrono>
#include <random>
#include <thread>

namespace {

    // --- настройки: меняй здесь, а не по всему коду ---
    const std::string BREAKFAST_CATEGORY = "Breakfast";
    // Внимание: поиск идёт по подстроке в названии, поэтому "oat" нашёл бы и "Goat ...".
    const std::vector<std::string> BREAKFAST_KEYWORDS = {
        "pancake", "omelette", "porridge", "waffle", "toast", "oatmeal", "crepe", "granola"
    };
    constexpr std::size_t MIN_CANDIDATES = 6;   // сколько подходящих блюд достаточно собрать
    constexpr std::size_t MAX_LOOKUPS = 12;     // максимум запросов "блюдо по id" за один подбор
    constexpr std::size_t TOP_N = 3;            // выбираем случайно среди N лучших
    const auto PAUSE = std::chrono::milliseconds(150); // пауза между запросами

    std::string lower(std::string s) {
        for (char& c : s) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
        return s;
    }

    // Есть ли word внутри text (без учёта регистра). Пустые и "пробельные" слова игнорируются,
    // иначе случайный пробел в списке запретов "запретил" бы все блюда.
    bool contains_ci(const std::string& text, const std::string& word) {
        if (word.find_first_not_of(" \t\r\n") == std::string::npos) return false;
        return lower(text).find(lower(word)) != std::string::npos;
    }

    // Всё, что блюдо содержать не должно: аллергены и "точно не ем".
    // ВАЖНО: здесь должны лежать английские названия ингредиентов. Если в профиле хранятся
    // группы ("nuts"), раскрой их в список ингредиентов именно в этой функции.
    std::vector<std::string> banned_list(const User& user) {
        std::vector<std::string> banned = user.allergens;
        banned.insert(banned.end(), user.disliked_products.begin(), user.disliked_products.end());
        return banned;
    }

} // namespace

bool Breakfast::is_allowed(const Dish& dish, const std::vector<std::string>& banned) {
    if (dish.ingredients.empty()) return false;
    for (const auto& ingredient : dish.ingredients) {
        for (const auto& word : banned) {
            if (contains_ci(ingredient, word)) return false;
        }
    }
    return true;
}

double Breakfast::score(const Dish& dish, const User& user) {
    double result = 0.0;
    for (const auto& liked : user.liked_products) {
        for (const auto& ingredient : dish.ingredients) {
            if (contains_ci(ingredient, liked)) {
                result += 1.0;
                break; // один любимый продукт считаем один раз
            }
        }
    }
    return result;
}

std::vector<Dish> Breakfast::find_candidates(const User& user,
    const std::unordered_set<std::string>& already_shown) {

    const std::vector<std::string> banned = banned_list(user);
    std::vector<Dish> result;
    std::unordered_set<std::string> seen = already_shown; // показанные + уже обработанные

    // Проверяет блюдо один раз и добавляет, если оно подходит
    auto consider = [&](const Dish& dish) {
        if (dish.id.empty() || !seen.insert(dish.id).second) return;
        if (is_allowed(dish, banned)) result.push_back(dish);
        };

    // Источник 1: категория Breakfast (приходят только id, ингредиенты запрашиваем отдельно)
    const std::vector<std::string> ids = MealDB::filter_ids_by_category(BREAKFAST_CATEGORY);
    std::size_t lookups = 0;
    for (const auto& id : ids) {
        if (result.size() >= MIN_CANDIDATES || lookups >= MAX_LOOKUPS) break;
        if (seen.count(id)) continue;
        std::this_thread::sleep_for(PAUSE);
        ++lookups;
        if (auto dish = MealDB::lookup_by_id(id)) consider(*dish);
    }

    // Источник 2: поиск по ключевым словам (в ответе сразу полные данные)
    for (const auto& word : BREAKFAST_KEYWORDS) {
        if (result.size() >= MIN_CANDIDATES) break;
        std::this_thread::sleep_for(PAUSE);
        for (const auto& dish : MealDB::search_by_name(word)) consider(dish);
    }

    return result;
}

std::optional<Dish> Breakfast::suggest(const User& user,
    std::unordered_set<std::string>& already_shown) {

    std::vector<Dish> candidates = find_candidates(user, already_shown);
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