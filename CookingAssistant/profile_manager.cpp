#include "profile_manager.h"
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

void ProfileManager::save_user_to_file(const User& user) {
    const json profile = {
        {"name", user.name}, {"age", user.age}, {"gender", user.gender},
        {"weight", user.weight}, {"height", user.height}, {"target", user.target},
        {"activity_level", user.activity_level}, {"calorie_norm", user.calorie_norm},
        {"allergens", user.allergens}, {"liked_products", user.liked_products},
        {"disliked_products", user.disliked_products}
    };
    std::ofstream file("profile.json");
    if (!file) { std::cerr << "Failed to save profile!\n"; return; }
    file << profile.dump(4);
    std::cout << "Profile saved!\n";
}

void ProfileManager::read_user_from_file(User& user) {
    std::ifstream file("profile.json");
    if (!file) { std::cout << "Failed to open profile!\n"; return; }
    try {
        const json profile = json::parse(file);
        user.name = profile.value("name", std::string{});
        user.age = profile.value("age", 0);
        user.gender = profile.value("gender", std::string("not_chosen"));
        user.weight = profile.value("weight", 0.0);
        user.height = profile.value("height", 0.0);
        user.target = profile.value("target", std::string("not_chosen"));
        user.activity_level = profile.value("activity_level", std::string("not_chosen"));
        user.calorie_norm = profile.value("calorie_norm", 0);
        user.allergens = profile.value("allergens", std::vector<std::string>{});
        user.liked_products = profile.value("liked_products", std::vector<std::string>{});
        user.disliked_products = profile.value("disliked_products", std::vector<std::string>{});
        std::cout << "Profile loaded successfully!\n";
    } catch (const json::exception& e) {
        std::cerr << "Invalid profile JSON: " << e.what() << '\n';
    }
}
