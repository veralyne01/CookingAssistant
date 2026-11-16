
#include "MeasureConverter.h"

#include <regex>
#include <algorithm>
#include <cctype>
#include <unordered_map>
#include <string>
#include <optional>

namespace MeasureConverter {

    namespace {

        // Приводим строку к нижнему регистру.
        std::string normalize(std::string value) {
            std::transform(
                value.begin(),
                value.end(),
                value.begin(),
                [](unsigned char c) {
                    return static_cast<char>(std::tolower(c));
                }
            );

            return value;
        }

        // Убираем пробелы по краям.
        std::string trim(const std::string& value) {
            const size_t first = value.find_first_not_of(" \t\r\n");

            if (first == std::string::npos)
                return "";

            const size_t last = value.find_last_not_of(" \t\r\n");

            return value.substr(first, last - first + 1);
        }

        bool contains(
            const std::string& name,
            const std::string& word
        ) {
            return name.find(word) != std::string::npos;
        }

        // Определяем единицу измерения по её обозначению.
        MeasureUnit identify_unit(const std::string& raw_unit) {
            const std::string unit = normalize(raw_unit);

            static const std::unordered_map<std::string, MeasureUnit> units = {

                // Масса
                {"g", MeasureUnit::GRAM},
                {"gram", MeasureUnit::GRAM},
                {"grams", MeasureUnit::GRAM},

                {"kg", MeasureUnit::KILOGRAM},
                {"kilogram", MeasureUnit::KILOGRAM},
                {"kilograms", MeasureUnit::KILOGRAM},

                {"oz", MeasureUnit::OUNCE},
                {"ounce", MeasureUnit::OUNCE},
                {"ounces", MeasureUnit::OUNCE},

                {"lb", MeasureUnit::POUND},
                {"lbs", MeasureUnit::POUND},
                {"pound", MeasureUnit::POUND},
                {"pounds", MeasureUnit::POUND},

                // Объём
                {"ml", MeasureUnit::MILLILITER},
                {"milliliter", MeasureUnit::MILLILITER},
                {"milliliters", MeasureUnit::MILLILITER},
                {"millilitre", MeasureUnit::MILLILITER},
                {"millilitres", MeasureUnit::MILLILITER},

                {"l", MeasureUnit::LITER},
                {"liter", MeasureUnit::LITER},
                {"liters", MeasureUnit::LITER},
                {"litre", MeasureUnit::LITER},
                {"litres", MeasureUnit::LITER},

                {"tsp", MeasureUnit::TEASPOON},
                {"teaspoon", MeasureUnit::TEASPOON},
                {"teaspoons", MeasureUnit::TEASPOON},

                {"tbs", MeasureUnit::TABLESPOON},
                {"tbsp", MeasureUnit::TABLESPOON},
                {"tablespoon", MeasureUnit::TABLESPOON},
                {"tablespoons", MeasureUnit::TABLESPOON},

                {"cup", MeasureUnit::CUP},
                {"cups", MeasureUnit::CUP},

                {"fl oz", MeasureUnit::FLUID_OUNCE},
                {"fluid ounce", MeasureUnit::FLUID_OUNCE},
                {"fluid ounces", MeasureUnit::FLUID_OUNCE},

                // Количество
                {"piece", MeasureUnit::PIECE},
                {"pieces", MeasureUnit::PIECE},
                {"pc", MeasureUnit::PIECE},
                {"pcs", MeasureUnit::PIECE},

                {"slice", MeasureUnit::SLICE},
                {"slices", MeasureUnit::SLICE},

                {"strip", MeasureUnit::STRIP},
                {"strips", MeasureUnit::STRIP},

                {"clove", MeasureUnit::CLOVE},
                {"cloves", MeasureUnit::CLOVE},

                {"small", MeasureUnit::SMALL},
                {"medium", MeasureUnit::MEDIUM},
                {"large", MeasureUnit::LARGE},

                {"pinch", MeasureUnit::PINCH},
                {"pinches", MeasureUnit::PINCH}
            };

            auto it = units.find(unit);

            if (it == units.end())
                return MeasureUnit::UNKNOWN;

            return it->second;
        }

        // Плотность продукта в г/мл.
        // Значения приблизительные.
        std::optional<double> get_density(
            const std::string& raw_name
        ) {
            const std::string name = normalize(raw_name);

            if (contains(name, "olive oil") ||
                contains(name, "vegetable oil") ||
                contains(name, "sunflower oil") ||
                contains(name, "canola oil")) {
                return 0.91;
            }

            if (contains(name, "milk") ||
                contains(name, "cream")) {
                return 1.03;
            }

            if (contains(name, "maple syrup"))
                return 1.33;

            if (contains(name, "honey"))
                return 1.42;

            if (contains(name, "salt"))
                return 1.20;

            if (contains(name, "flour"))
                return 0.53;

            if (contains(name, "sugar"))
                return 0.85;

            if (contains(name, "butter"))
                return 0.96;

            if (contains(name, "water"))
                return 1.0;

            if (contains(name, "soy sauce"))
                return 1.05;

            if (contains(name, "vinegar"))
                return 1.01;

            if (contains(name, "lemon juice"))
                return 1.03;

            return std::nullopt;
        }

        // Приблизительная масса одной единицы продукта.
        std::optional<double> get_unit_weight(
            const std::string& raw_name,
            MeasureUnit unit
        ) {
            const std::string name = normalize(raw_name);

            // Яйца.
            if (contains(name, "egg") &&
                (unit == MeasureUnit::PIECE ||
                    unit == MeasureUnit::SMALL ||
                    unit == MeasureUnit::MEDIUM ||
                    unit == MeasureUnit::LARGE)) {

                if (unit == MeasureUnit::SMALL)
                    return 40.0;

                if (unit == MeasureUnit::LARGE)
                    return 60.0;

                return 50.0;
            }

            // Картофель.
            if (contains(name, "potato")) {
                if (unit == MeasureUnit::SMALL)
                    return 100.0;

                if (unit == MeasureUnit::MEDIUM)
                    return 150.0;

                if (unit == MeasureUnit::LARGE)
                    return 250.0;

                if (unit == MeasureUnit::PIECE)
                    return 150.0;
            }

            // Чеснок.
            if (contains(name, "garlic") &&
                unit == MeasureUnit::CLOVE) {
                return 3.0;
            }

            // Бекон.
            if (contains(name, "bacon") &&
                unit == MeasureUnit::STRIP) {
                return 8.0;
            }

            // Хлеб.
            if (contains(name, "bread") &&
                unit == MeasureUnit::SLICE) {
                return 30.0;
            }

            // Петрушка.
            if (contains(name, "parsley") &&
                unit == MeasureUnit::PIECE) {
                return 2.0;
            }

            return std::nullopt;
        }

        // Приблизительная масса одной щепотки.
        std::optional<double> get_pinch_weight(
            const std::string& raw_name
        ) {
            const std::string name = normalize(raw_name);

            if (contains(name, "salt"))
                return 0.36;

            if (contains(name, "pepper"))
                return 0.15;

            if (contains(name, "allspice") ||
                contains(name, "cinnamon") ||
                contains(name, "paprika")) {
                return 0.20;
            }

            return std::nullopt;
        }
    }

    // -------------------------------------------------
    // PARSE MEASURE
    // -------------------------------------------------

    std::optional<ParsedMeasure> parse_measure(
        const std::string& raw_measure
    ) {
        const std::string measure = normalize(trim(raw_measure));

        if (measure.empty())
            return std::nullopt;

        // Меры без числа.
        // Считаем, что Pinch означает одну щепотку.
        if (measure == "pinch" || measure == "pinches") {
            return ParsedMeasure{
                1.0,
                MeasureUnit::PINCH
            };
        }

        // Число + единица измерения.
        // Поддерживает: 200g, 0.5 kg, 1 tbs, 2 strips.
        static const std::regex pattern(
            R"(^\s*(\d+(?:[.,]\d+)?)\s*([a-z ]*)\s*$)"
        );

        std::smatch match;

        if (!std::regex_match(measure, match, pattern))
            return std::nullopt;

        std::string number = match[1].str();
        std::string unit_string = trim(match[2].str());

        std::replace(
            number.begin(),
            number.end(),
            ',',
            '.'
        );

        double amount;

        try {
            amount = std::stod(number);
        }
        catch (...) {
            return std::nullopt;
        }

        if (amount <= 0)
            return std::nullopt;

        // Если единицы нет, трактуем число как количество штук.
        MeasureUnit unit = unit_string.empty()
            ? MeasureUnit::PIECE
            : identify_unit(unit_string);

        if (unit == MeasureUnit::UNKNOWN)
            return std::nullopt;

        return ParsedMeasure{
            amount,
            unit
        };
    }

    // -------------------------------------------------
    // CONVERT TO GRAMS
    // -------------------------------------------------

    std::optional<WeightResult> convert_to_grams(
        const std::string& ingredient_name,
        const ParsedMeasure& measure
    ) {
        const double amount = measure.amount;

        if (amount <= 0)
            return std::nullopt;

        // Масса: точное преобразование.
        switch (measure.unit) {

        case MeasureUnit::GRAM:
            return WeightResult{ amount, false };

        case MeasureUnit::KILOGRAM:
            return WeightResult{ amount * 1000.0, false };

        case MeasureUnit::OUNCE:
            return WeightResult{ amount * 28.3495, false };

        case MeasureUnit::POUND:
            return WeightResult{ amount * 453.592, false };

        default:
            break;
        }

        // Объём: сначала переводим в миллилитры.
        double volume_ml = 0.0;

        switch (measure.unit) {

        case MeasureUnit::MILLILITER:
            volume_ml = amount;
            break;

        case MeasureUnit::LITER:
            volume_ml = amount * 1000.0;
            break;

        case MeasureUnit::TEASPOON:
            volume_ml = amount * 5.0;
            break;

        case MeasureUnit::TABLESPOON:
            volume_ml = amount * 15.0;
            break;

        case MeasureUnit::CUP:
            volume_ml = amount * 240.0;
            break;

        case MeasureUnit::FLUID_OUNCE:
            volume_ml = amount * 29.5735;
            break;

        default:
            break;
        }

        if (volume_ml > 0) {
            auto density = get_density(ingredient_name);

            if (!density.has_value())
                return std::nullopt;

            return WeightResult{
                volume_ml * density.value(),
                true
            };
        }

        // Щепотка.
        if (measure.unit == MeasureUnit::PINCH) {
            auto weight = get_pinch_weight(ingredient_name);

            if (!weight.has_value())
                return std::nullopt;

            return WeightResult{
                amount * weight.value(),
                true
            };
        }

        // Штучные продукты.
        auto unit_weight = get_unit_weight(
            ingredient_name,
            measure.unit
        );

        if (!unit_weight.has_value())
            return std::nullopt;

        return WeightResult{
            amount * unit_weight.value(),
            true
        };
    }

    // -------------------------------------------------
    // COMPLETE CONVERSION
    // -------------------------------------------------

    std::optional<WeightResult> get_weight(
        const std::string& ingredient_name,
        const std::string& measure
    ) {
        auto parsed = parse_measure(measure);

        if (!parsed.has_value())
            return std::nullopt;

        return convert_to_grams(
            ingredient_name,
            parsed.value()
        );
    }
}
