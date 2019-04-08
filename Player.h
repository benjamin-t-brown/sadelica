#pragma once

#include <string>

#include "Game.h"

class Item;

void build_player();

namespace player
{
extern std::vector<std::string> item_storage;
extern std::vector<std::string> quest_items;
extern int selected_character;
extern int max_inventory_slots;

std::string save_player_data();
std::string save_item_storage();
Character* get_leader();
std::map<std::string, std::string>& get_player_data();
void load_party(std::vector<Character*>& party);
void load_alive_party(std::vector<Character*>& party);
void load_spells(Character* ch, std::vector<std::string>& spells);
void load_inventory(Character* ch, std::vector<std::string>& inventory);

void move_party_members();
void add_spell(std::string spell_name, Character* ch);
void remove_spell(std::string spell_name, Character* ch);
void store_item(std::string item_name, bool log_item = false);
void unstore_item(std::string item_name, bool log_item = false);
void print_storage();
bool add_item(std::string item_name, Character* ch);
bool remove_item(std::string item_name, Character* ch);
bool remove_item_ind(int ind, Character* ch);
bool has_item(std::string item_name);
void set_at(int x, int y, int i, int j);

void add_quest_item(std::string item_name);
bool remove_quest_item(std::string item_name);
bool has_quest_item(std::string item_name);

bool select_character(int i);
void transfer_item(Character* from, int ind, Character* to, int ind2);
void modify_coins(int v);
void set(std::string key, std::string val = "yes");
std::string get_string(std::string key);
bool get(std::string key);
bool has(std::string key);
int get_int(std::string key);
void set_int(std::string key, int v);

Character* get_selected_character();

void pick_party_member(std::string text, std::function<void(Character*)> cb);
void pick_item_from_inventory(std::string text, Character* ch, std::function<void(Item*,int)> cb);
void pick_item_from_storage(std::string text, int filter_usable, std::function<void(Item*,int)> cb);
void pick_inventory_slot(std::string text, Character* ch, std::function<void(int)> cb);

void show_inventory_menu();

void log_inventory(Character* ch);
void log_character(Character* ch);
void log_storage();
void log_player();
void log_player_data();
void print_status_bar();
}