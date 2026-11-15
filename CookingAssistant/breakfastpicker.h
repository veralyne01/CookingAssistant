#pragma once
#include <optional>
#include <string>
#include <unordered_set>
#include <vector>
#include "dishes.h"
#include "user.h"

namespace Breakfast {

    // Жёсткий фильтр: false, если среди ингредиентов есть что-то из banned
    // (без учёта регистра, по части слова: "peanut" найдёт "Peanut Butter").
    // Блюдо без списка ингредиентов не проходит: проверить его нельзя.
    bool is_allowed(const Dish& dish, const std::vector<std::string>& banned);

    // Мягкая оценка: сколько любимых продуктов пользователя есть в блюде
    double score(const Dish& dish, const User& user);

    // Собирает блюда, прошедшие жёсткий фильтр. already_shown пропускаются.
    std::vector<Dish> find_candidates(const User& user,
        const std::unordered_set<std::string>& already_shown);

    // Предлагает одно блюдо и запоминает его id в already_shown (для кнопки "другое").
    // std::nullopt, если подходящих блюд нет (или нет интернета).
    std::optional<Dish> suggest(const User& user,
        std::unordered_set<std::string>& already_shown);

}