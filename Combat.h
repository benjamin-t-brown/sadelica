#pragma once

#include <functional>
#include <string>
#include <vector>

class Character;

#define COMBAT_MAX_CHARACTERS 25
#define COMBAT_XMAPSIZE 25
#define COMBAT_YMAPSIZE 25

#define COMBAT_BASE_AP 6

class StatusEffect;

namespace combat
{
extern int current_character_index;
extern bool ai_acting;
extern std::vector<StatusEffect*> status_effects;
extern int turn_number;

void begin_combat(std::string encounter_name);
void end_combat();

int calculate_melee_damage(int dmg, Character* attacker, Character* victim);
int calculate_magic_damage(int dmg, Character* attacker, Character* victim);

bool attack(Character* attacker, Character* victim);
bool perform_magic(Character* ch, std::string spell, std::vector<std::string&> targets);
bool move_character(Character* ch, int x_offset, int y_offset);
void add_status_effect(StatusEffect* se);

void choose_magic();

void set_next_character_index();
void after_action();
void after_turn();

Character* get_active_character();
void check_health();
bool do_enemies_remain();
bool do_players_remain();
void get_adjacent_enemies(Character* ch, std::vector<Character*>& out);

void handle_keypress(char key);

void update();
}