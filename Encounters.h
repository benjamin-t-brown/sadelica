#pragma once

#include "Game.h"
#include <memory>

#define ENCOUNTER_MAX_ENEMIES 50

class Encounter
{
  public:
	std::string name;
	int world_x;
	int world_y;
	int x;
	int y;
	std::string behavior;

	Encounter(std::string n, int world_x, int world_y, int x, int y, std::string behavior)
		: name(n), world_x(world_x), world_y(world_y), x(x), y(y), behavior(behavior) {}
	Encounter(Encounter* enc);
	std::string save();
};

void build_encounters();

namespace encounters
{
extern bool disable_encounters;
extern int world_x;
extern int world_y;
extern Character* enemies[GAME_MAX_CHARACTERS];
extern std::map<std::string, std::function<void()>> stored_encounters;
extern std::map<std::string, std::vector<std::shared_ptr<Encounter>>> active_encounters;
std::vector<std::shared_ptr<Encounter>>& get_encounters_at(int world_x, int world_y);
void add_encounter(std::string key, std::function<void()> cb);
void add_encounter_to_map(Encounter* enc);
void remove_encounter_from_map(Encounter* enc);
void create_encounter(std::string name, int world_x, int world_y, int x, int y, std::string behavior = "random");
void assign_enemies(Character* enemies[ENCOUNTER_MAX_ENEMIES]);
void clear_character_array(Character* arr[]);
void initiate_encounter(Encounter* enc);
Encounter* get_encounter_at(int world_x, int world_y, int x, int y);
Encounter* get_encounter_nearby(Character* ch, int n);
bool print_encounter(int x, int y);
void set_encounter(std::string key);
void update_encounters();
}