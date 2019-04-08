#pragma once

#include <functional>
#include <map>
#include <string>
#include <vector>

class Character;

class StatusEffect
{
  public:
	int max_turns;
	int turn;
	Character* ch;
	bool should_remove;
	std::function<void(Character*)> begin_cb;
	std::function<void(Character*)> end_cb;
	std::string name;
	std::string icon;

	StatusEffect(
		std::string name,
		std::string icon,
		Character* ch,
		std::function<void(Character*)> begin_cb,
		std::function<void(Character*)> end_cb);

	void on_turn_end();
	void on_turn_begin();
};

void build_magic();

namespace magic
{
void cast_spell(Character* ch, std::string spell, std::function<void()> cb);
StatusEffect* create_status_effect(std::string name, Character* ch, int num_turns = 3);
void show_positive_status(std::string name, std::vector<Character*>& targets);
void show_negative_status(std::string name, std::vector<Character*>& targets);
void set_cpu_targeting_func(std::function<std::vector<Character*>()> func);
extern std::function<std::vector<Character*>()> cpu_targeting_func;
}