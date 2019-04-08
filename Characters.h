#pragma once

#include "Game.h"

#define CHARACTER_LIST_LENGTH 1000

void load_characters(int world_x, int world_y, Character* arr[GAME_MAX_CHARACTERS]);
void build_characters();
Character* get_character(int i);
Character* get_character(std::string name);

class Character
{
  public:
	std::string name;
	int world_x;
	int world_y;
	int old_x;
	int old_y;
	int x;
	int y;
	std::vector<int> stored_coords;
	std::string old_color;
	std::string old_bgcolor;
	std::string old_text;
	std::string color;
	std::string bgcolor;
	std::string text;
	std::string behavior;

	bool is_cpu;

	int base_talk_index;
	int talk_index;
	std::string talk_text;
	std::vector<std::function<void(Character*)>> dialogs;
	std::map<std::string, int> dialog_mapping;

	int max_ap;
	int ap;
	int max_hp;
	int hp;
	int armor_class;
	int bonus_armor_class;
	int attack;
	int bonus_attack;
	int max_damage;
	int bonus_max_damage;
	int dmg_variance_percent;
	int magic_dmg_trust_deviation;
	int magic;
	int bonus_magic;
	int damage_reduction;
	int bonus_damage_reduction;
	int effect_damage_reduction;

	std::vector<std::string> spells;
	std::vector<std::string> items;

	Character(
		std::string name,
		std::string text,
		std::string color,
		std::string bgcolor,
		int world_x,
		int world_y,
		int x, 
		int y,
		std::string behavior);
	Character(
		std::string name,
		std::string text,
		std::string color,
		std::string bgcolor,
		int world_x,
		int world_y,
		int x,
		int y);
	Character();

	void init(
		std::string name,
		std::string text,
		std::string color,
		std::string bgcolor,
		int world_x,
		int world_y,
		int x,
		int y,
		std::string behavior);
	int get_talk_index(std::function<void(Character*)>);
	void print();
	void act();

	void store_coords();
	void restore_coords();
	std::vector<int> get();
	void set(int wx, int wy, int lx, int ly);
	std::string get_str_position();
	void hide();

	void reset_sprite();
	void set_sprite(std::string text, std::string color, std::string bgcolor);

	void reset_ap();
	void reset_hp();
	void modify_hp(int n);
	bool is_dead();

	bool is_on_current_map();
	void set_talk_index(int i);
	void set_talk_index(std::string str);
	void set_base_talk_index(int i);
	void set_base_talk_index(std::string str);
	void add_dialog(std::function<void(Character*)> cb, std::string label = "");
	std::string save();
	std::string save_player_character();
	void load(std::string obj);
};

namespace characters
{
void show_store(std::string store_name);
void show_sell();
void add_character(Character* ch);
void ultimatum(std::string text, Character* ch, std::function<void()> yes_cb, std::function<void()> no_cb);
}
