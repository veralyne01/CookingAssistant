#pragma once 
#include <iostream>
#include <vector>
#include <chrono>
namespace Auxiliary{
    const std::vector<std::string> BREAKFAST_KEYWORDS = {
       "pancake", "omelette", "porridge", "waffle", "toast", "oatmeal", "crepe", "granola", "cottage"
    };
    const std::vector<std::string> LUNCH_KEYWORDS = {
        "soup", "fish",
    };
    const std::vector<std::string> DINNER_KEYWORDS = {
        "pancake", "omelette", "porridge", "waffle", "toast", "oatmeal", "crepe", "granola"
    };
    const std::vector<std::string> SNACK_KEYWORDS = {
        "pancake", "omelette", "porridge", "waffle", "toast", "oatmeal", "crepe", "granola"
    };
    constexpr std::size_t MIN_CANDIDATES = 20;   
    constexpr std::size_t MAX_LOOKUPS = 40;     
    const auto PAUSE = std::chrono::milliseconds(150); 

    std::string lower(std::string s);
    bool contains_ci(const std::string& text, const std::string& word);

}


