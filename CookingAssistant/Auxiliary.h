#pragma once 
#include <iostream>
#include <vector>
#include <chrono>
#include <fstream>
#include <optional>
#include <string>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
namespace Auxiliary {
    const auto PAUSE = std::chrono::milliseconds(150); 

    std::string lower(std::string s);
    bool contains_ci(const std::string& text, const std::string& word);
    json load_json(const std::string& filename);
    std::optional<std::vector<int>> parse_number_line(
        std::string& input,
        int min_value,
        int max_value);
    void print_string_vector(const std::vector<std::string>& list);
}