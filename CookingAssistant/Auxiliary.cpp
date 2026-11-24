#include "auxiliary.h"
#include <algorithm>
#include <cctype>
#include <stdexcept>
namespace Auxiliary {
    std::string lower(std::string s) {
        for (char& c : s) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
        return s;
    }
    bool contains_ci(const std::string& text, const std::string& word) {
        if (word.find_first_not_of(" \t\r\n") == std::string::npos) return false;
        return lower(text).find(lower(word)) != std::string::npos;
    }
    json load_json(const std::string& filename) {
        std::ifstream file(filename);
        std::cout << "Current directory: "
            << std::filesystem::current_path() << '\n';
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open file: " + filename);
        }

        try {
            return json::parse(file);
        }
        catch (const json::parse_error& e) {
            throw std::runtime_error(
                "Invalid JSON in " + filename + ": " + e.what()
            );
        }
    }
    std::optional<std::vector<int>> parse_number_line(
        const std::string& input,
        int min_value,
        int max_value)
    {
        std::istringstream stream(input);
        std::vector<int> numbers;

        std::string token;

        while (stream >> token)
        {
            try
            {
                size_t pos;
                int number = std::stoi(token, &pos);

                if (pos != token.size())
                    return std::nullopt;

                if (number < min_value || number > max_value)
                    return std::nullopt;

                for (int existing : numbers)
                {
                    if (existing == number)
                        return std::nullopt;
                }

                numbers.push_back(number);
            }
            catch (...)
            {
                return std::nullopt;
            }
        }

        if (numbers.empty())
            return std::nullopt;

        return numbers;
    }
}
