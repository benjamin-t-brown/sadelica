#pragma once

#include <functional>
#include <map>
#include <string>
#include <vector>

#define ITEM_EMPTY_FUNC [](Character*) {}

class Character;
class Store;

class Item
{
  public:
  	float weight;
  	bool usable;
  	bool ephemeral;
  	std::string usable_type;
	std::string name;
	std::string description;

	std::function<void(Character*)> on_acquire;
	std::function<void(Character*)> on_unacquire;
	std::function<void(Character*)> on_use;
	std::function<void()> on_acquire_ephemeral;

	Item();
	std::string get_text();
	void set_usable(std::string type, std::function<void(Character*)> cb);
	void set_ephemeral(std::function<void()> cb);
	bool is_usable_now();
};

class Store
{
  public:
	std::string name;
	std::map<std::string, int> items;

	Store(std::string store_name);

	void put_item_in_store(std::string name, int cost);
};

Item* get_item(std::string item_name);
void load_store(std::string store_name, std::vector<std::pair<std::string, int>>& store);
void build_items();
void build_stores();
void add_item(Item* item);
void add_store(Store* store);
