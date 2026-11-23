#pragma once 
#include <iostream>
#include <vector>
#include <chrono>
#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
namespace Auxiliary {
    const std::vector<std::string> BREAKFAST_KEYWORDS = {
       "pancake", "omelette", "porridge", "waffle", "toast", "oatmeal", "crepe", "granola", "cottage"
    };
    const std::vector<std::string> LUNCH_KEYWORDS = {
        "soup", "fish", "chicken", "beef", "pasta", "salad", "rice", "curry"
    };
    const std::vector<std::string> DINNER_KEYWORDS = {
        "chicken", "beef", "fish", "pasta", "curry", "stew", "roast", "casserole"
    };
    const std::vector<std::string> SNACK_KEYWORDS = {
        "sandwich", "salad", "toast", "smoothie", "fruit", "muffin", "snack", "cake"
    };
    const auto PAUSE = std::chrono::milliseconds(150); 

    std::string lower(std::string s);
    bool contains_ci(const std::string& text, const std::string& word);
    json load_json(const std::string& filename);
}


