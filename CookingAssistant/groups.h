#pragma once
#include <vector>
#include <iostream>
struct Group {                       
	std::string label;                    
	std::vector<std::string> ingredients;      
	std::vector<std::string> categories;       
};
const std::vector<Group> ALLERGIES = {
	{"Nuts", {"Almonds","Ground Almonds","Flaked Almonds","Walnuts","Cashews","Hazelnuts","Pecan Nuts","Pistachios","Pine Nuts","Macadamia Nuts","Chestnuts","Marzipan"}, {}},
	{"Peanuts", {"Peanuts","Peanut Butter","Peanut Oil"}, {}},
	{"Dairy", {"Milk","Butter","Ghee","Double Cream","Single Cream","Heavy Cream","Sour Cream","Creme Fraiche","Condensed Milk","Evaporated Milk","Yogurt","Greek Yogurt","Cheese","Cheddar Cheese","Parmesan","Mozzarella","Feta","Ricotta","Mascarpone","Cream Cheese","Custard","Whipped Cream"}, {}},
	{"Eggs", {"Eggs","Egg Yolks","Egg White","Mayonnaise"}, {}},
	{"Gluten", {"Flour","Plain Flour","Self-raising Flour","Bread","Breadcrumbs","Spaghetti","Penne","Macaroni","Lasagne Sheets","Noodles","Couscous","Bulgur Wheat","Barley","Rye","Puff Pastry","Filo Pastry","Tortilla","Pita Bread","Soy Sauce"}, {}},
	{"Seafood", {"Prawns","King Prawns","Shrimp","Salmon","Tuna","Cod","Haddock","Sardines","Anchovies","Mussels","Clams","Squid","Crab","Lobster","Oysters","Scallops","Fish Sauce","Fish Stock"}, {"Seafood"}},
	{"Soy", {"Soy Sauce","Tofu","Miso","Soybeans","Edamame","Soy Milk"}, {}},
	{"Chicken", {"Chicken","Chicken Breast","Chicken Thighs","Chicken Legs","Chicken Wings","Chicken Stock"}, {"Chicken"}},
};

const std::vector<std::string> VEG_BAD = { "Bacon","Ham","Sausages","Chorizo","Gelatine","Lard","Anchovies","Fish Sauce","Chicken Stock","Beef Stock","Worcestershire Sauce" };
const std::vector<std::string> VEGAN_BAD = { "Milk","Butter","Ghee","Double Cream","Single Cream","Heavy Cream","Sour Cream","Cheese","Yogurt","Eggs","Egg Yolks","Honey","Mayonnaise","Condensed Milk","Custard","Parmesan","Feta","Mozzarella" };

const std::vector<Group> DIETS = {
	{"All", {}, {}},
	{"Vegetarian ", VEG_BAD, {"Beef","Chicken","Pork","Lamb","Goat","Seafood"}},
	{"Vegan", [] { auto v = VEG_BAD; v.insert(v.end(), VEGAN_BAD.begin(), VEGAN_BAD.end()); return v; }(), {"Beef","Chicken","Pork","Lamb","Goat","Seafood"}},
	{"No pork", {"Pork","Pork Chops","Bacon","Pancetta","Ham","Gammon","Prosciutto","Salami","Pepperoni","Chorizo","Sausages","Lard","Black Pudding","Gelatine"}, {"Pork"}},
	{"No beef", {"Beef","Minced Beef","Beef Brisket","Beef Fillet","Beef Shin","Beef Stock","Beef Gravy","Oxtail","Veal"}, {"Beef"}},
};

const std::vector<Group> CUISINES = {    
	{"Italian", {}, {"Italian"}},
	{"Asian", {}, {"Chinese","Japanese","Thai","Vietnamese","Malaysian","Filipino","Indian"}},
	{"Mexican", {}, {"Mexican"}},
	{"American", {}, {"American","Canadian"}},
	{"Greek", {}, {"Greek"}},
	{"Spanish", {}, {"Spanish"}},
	{"French", {}, {"French"}},
	{"Russian", {}, {"Russian"}},
	{"Polish", {}, {"Polish"}},
	{"Turkish", {}, {"Turkish"}},
	{"Indian", {}, {"Indian"}},
};

const std::vector<Group> FAVORITES = {  
	{"Meat", {}, {"Beef","Pork","Lamb","Goat"}},
	{"Chicken", {}, {"Chicken"}},
	{"Fish and seafood", {}, {"Seafood"}},
	{"Vegetables", {}, {"Vegetarian","Vegan"}},
	{"Pasta and cereals", {}, {"Pasta"}},
	{"Sweet", {}, {"Dessert"}},
};