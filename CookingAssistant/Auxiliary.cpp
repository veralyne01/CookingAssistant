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
}