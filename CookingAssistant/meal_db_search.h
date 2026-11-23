#pragma once
#include "dishes.h"
#include <optional>
#include <string>
#include <vector>
#include "user.h"

// Клиент TheMealDB. При сетевой ошибке или неожиданном ответе
// функции возвращают пустой результат и не бросают исключений.
namespace MealDB {
    // filter.php?c=...  -> только id блюд категории (без ингредиентов)
    std::vector<std::string> filter_ids_by_category(const std::string& category);

    // search.php?s=...  -> полные данные блюд, в названии которых есть query
    std::vector<Dish> search_by_name(const std::string& query);

    // lookup.php?i=...  -> полные данные одного блюда
    std::optional<Dish> lookup_by_id(const std::string& id);

}
