
#pragma once

#include <optional>
#include <string>

namespace MeasureConverter {

    enum class MeasureUnit {
        GRAM,
        KILOGRAM,
        OUNCE,
        POUND,

        MILLILITER,
        LITER,
        TEASPOON,
        TABLESPOON,
        CUP,
        FLUID_OUNCE,

        PIECE,
        SLICE,
        STRIP,
        CLOVE,
        SMALL,
        MEDIUM,
        LARGE,

        PINCH,
        UNKNOWN
    };

    struct ParsedMeasure {
        double amount = 0.0;
        MeasureUnit unit = MeasureUnit::UNKNOWN;
    };

    struct WeightResult {
        double grams = 0.0;
        bool estimated = false;
    };

    // Разбирает строку с мерой.
    std::optional<ParsedMeasure> parse_measure(
        const std::string& measure
    );

    // Переводит распознанную меру в граммы.
    std::optional<WeightResult> convert_to_grams(
        const std::string& ingredient_name,
        const ParsedMeasure& measure
    );

    // Полный процесс: строка меры -> граммы.
    std::optional<WeightResult> get_weight(
        const std::string& ingredient_name,
        const std::string& measure
    );
}
