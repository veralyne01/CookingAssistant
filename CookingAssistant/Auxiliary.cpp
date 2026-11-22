#include "Auxiliary.h"
namespace Auxuliary {
    std::string lower(std::string s) {
        for (char& c : s) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
        return s;
    }
    bool contains_ci(const std::string& text, const std::string& word) {
        if (word.find_first_not_of(" \t\r\n") == std::string::npos) return false;
        return lower(text).find(lower(word)) != std::string::npos;
    }
}