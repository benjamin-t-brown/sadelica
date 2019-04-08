#include "Combat.h"
#include "AI.h"
#include "Characters.h"
#include "DrawText.h"
#include "Encounters.h"
#include "Game.h"
#include "Magic.h"
#include "Player.h"
#include "World.h"

#include <random>

std::pair<int,int> get_ordered_spawn_tile(int tile_id)
{
	for (int i = 0; i < COMBAT_YMAPSIZE; i++)
	{
		for (int j = 0; j < COMBAT_XMAPSIZE; j++)
		{
			if (game::map[i][j] == tile_id && game::get_character_at(j, i) == NULL)
			{
				return std::make_pair(j, i);
			}
		}
	}

	log("WARNING: No valid remaining position for spawn tile with id: " + std::to_string(tile_id));
	return std::make_pair(0, 0);
}

std::pair<int,int> get_random_spawn_tile(int tile_id)
{
	std::vector<std::pair<int,int>> positions;

	for (int i = 0; i < COMBAT_YMAPSIZE; i++)
	{
		for (int j = 0; j < COMBAT_XMAPSIZE; j++)
		{
			if (game::map[i][j] == tile_id && game::get_character_at(j, i) == NULL)
			{
				positions.push_back(std::make_pair(j, i));
			}
		}
	}

	if (positions.size())
	{
		return positions[game::get_random(0, positions.size())];
	}
	else
	{
		log("WARNING: No valid remaining position for spawn tile with id: " + std::to_string(tile_id));
		return std::make_pair(0, 0);
	}
}

void add_units_to_map(Character* list[10], int tile_id)
{
	for (int i = 0; i < 10; i++)
	{
		Character* ch = list[i];
		if (ch == NULL)
		{
			return;
		}
		std::pair<int,int> loc;
		loc = get_ordered_spawn_tile(tile_id);
		ch->store_coords();
		game::active_characters[combat::current_character_index] = ch;
		combat::current_character_index++;
		ch->world_x = encounters::world_x;
		ch->world_y = encounters::world_y;
		ch->x = loc.first;
		ch->y = loc.second;
		ch->reset_ap();
	}
}

Character* get_character_at(int x, int y)
{
	for (int i = 0; i < COMBAT_MAX_CHARACTERS; i++)
	{
		Character* c = game::active_characters[i];
		if (c != NULL && c->x == x && c->y == y)
		{
			return c;
		}
	}
	return NULL;
}

namespace combat
{

int current_character_index = 0;
bool ai_acting = false;
std::vector<StatusEffect*> status_effects;
int turn_number = 0;

void begin_combat(std::string encounter_name)
{
	turn_number = 0;
	for (int i = 0; i < COMBAT_MAX_CHARACTERS; i++)
	{
		game::active_characters[i] = NULL;
	}

	for (StatusEffect* se: status_effects)
	{
		delete se;
	}
	status_effects.clear();

	encounters::set_encounter(encounter_name);
	current_character_index = 0;
	world::load_map(encounters::world_x, encounters::world_y, game::map);
	game::current_world_x = encounters::world_x;
	game::current_world_y = encounters::world_y;

	Character* players[10];
	std::vector<Character*> party;
	player::load_alive_party(party);
	int ctr = 0;
	for (Character* ch: party)
		players[ctr++] = ch;
	for (int i = ctr; i < 10; i++)
		players[i] = NULL;

	add_units_to_map(players, -1000);
	add_units_to_map(encounters::enemies, -1001);
	combat::after_turn();
}

void end_combat()
{
	for (int i = 0; i < ENCOUNTER_MAX_ENEMIES; i++)
	{
		if (encounters::enemies[i] == NULL)
			break;

		delete encounters::enemies[i];
	}

	Character* player = player::get_leader();
	player->restore_coords();
	game::set_camera(player);
	game::change_map(player);
	game::on_encounter_end_cb();
}

int calculate_melee_damage(int dmg, Character* attacker, Character* victim)
{
	bool log_result = true;

	int d20 = game::get_random(1, 21);
	int att = attacker->attack + attacker->bonus_attack;
	int ac = victim->armor_class + victim->bonus_armor_class;
	int rdm = victim->damage_reduction + victim->bonus_damage_reduction;

	if (log_result)
	{
		std::stringstream ss;
		ss << " (" << d20 << " + " << att << ") " << (d20 + att) << " >= " << ac;
		log(ss.str());
	}
	if (d20 + att <= ac)
	{
		return -1;
	}
	else
	{
		return std::max(dmg - rdm, -1);
	}
}

int calculate_magic_damage(int dmg, Character* attacker, Character* victim)
{
	int mag = attacker->magic + attacker->bonus_magic;
	int rdm = victim->effect_damage_reduction + victim->bonus_damage_reduction;
	std::normal_distribution<double> distribution{0, (double)victim->magic_dmg_trust_deviation};
	std::default_random_engine generator;
	int number = round(distribution(generator));
	return std::max(dmg + mag + number - rdm, 0);
}

bool attack(Character* attacker, Character* victim)
{
	log(attacker->name + " attacks " + victim->name + "!");
	attacker->ap -= 4;
	int base_dmg = attacker->max_damage - game::get_random(0, (attacker->max_damage * attacker->dmg_variance_percent) / 100);
	int dmg = calculate_melee_damage(base_dmg, attacker, victim);

	//check for hit
	if (dmg > 0)
	{
		log(" " + std::to_string(dmg) + " damage!");
		victim->set_sprite("!", "white", "red");

		std::string text_dmg = std::to_string(dmg);

		game::add_floating_text(" " + text_dmg + " ", 4, victim->x - (text_dmg.size() / 2 + 1), victim->y - 1);
		game::add_action_seq([=]() {
			victim->reset_sprite();
		}, 1);
		game::add_action_seq([=]() {
			victim->modify_hp(-dmg);
		}, 2);
	}
	else
	{
		log(" missed!");
		victim->set_sprite("!", "white", "white");
		game::add_floating_text(" miss ", 4, victim->x - 3, victim->y - 1);
		game::add_action_seq([=]() {
			victim->reset_sprite();
		}, 1);
	}

	return true;
}

bool perform_magic(Character* ch)
{
	std::vector<std::string> spells;
	player::load_spells(ch, spells);
	return true;
}

bool move_character(Character* ch, int x_offset, int y_offset)
{
	ch->old_x = ch->x;
	ch->old_y = ch->y;

	Character* ch2 = get_character_at(ch->x + x_offset, ch->y + y_offset);
	if (ch2 != NULL)
	{
		if (ch2->is_cpu != ch->is_cpu)
		{
			return attack(ch, ch2);
		}
		else
		{
			return false;
		}
	}

	std::vector<Character*> prev_adjacent;
	get_adjacent_enemies(ch, prev_adjacent);

	ch->x += x_offset;
	ch->y += y_offset;

	std::vector<Character*> curr_adjacent;
	get_adjacent_enemies(ch, curr_adjacent);

	if (ch->x < 0)
	{
		ch->x = 0;
		return false;
	}
	else if (ch->x >= COMBAT_XMAPSIZE)
	{
		ch->x = COMBAT_XMAPSIZE - 1;
		return false;
	}
	else if (ch->y < 0)
	{
		ch->y = 0;
		return false;
	}
	else if (ch->y >= COMBAT_YMAPSIZE)
	{
		ch->y = COMBAT_YMAPSIZE - 1;
		return false;
	}
	else if (game::map[ch->y][ch->x] >= GAME_MOVE_THRESHOLD)
	{
		if (!ch->is_cpu)
		{
			log(" blocked! " + std::to_string(game::map[ch->x][ch->y]));
		}
		ch->x = ch->old_x;
		ch->y = ch->old_y;
		return false;
	}

	std::vector<Character*> diff_adjacent;
	std::copy_if(prev_adjacent.begin(), prev_adjacent.end(), std::back_inserter(diff_adjacent),
		[&](Character* c) {
			if (std::find(curr_adjacent.begin(), curr_adjacent.end(), c) != curr_adjacent.end())
			{
				return false;
			}
			else
			{
				return true;
			}
		});

	for (Character* c : diff_adjacent)
	{
		log("Attack of opportunity!");
		attack(c, ch);
	}

	ch->ap--;

	return true;
}

void add_status_effect(StatusEffect* se)
{
	status_effects.push_back(se);
}

void choose_magic()
{
	Character* ch = game::active_characters[current_character_index];
	std::vector<std::string> known_spells;
	player::load_spells(ch, known_spells);
	if (known_spells.size())
	{
		known_spells.push_back("Nevermind");
		game::allow_esc_while_waiting();
		game::wait_for_choice("Pick a spell:", known_spells, [=](int i) {
			if (i == (int)known_spells.size() || i == 0)
			{
				log(" Nevermind");
				return;
			}
			log(" " + known_spells[i - 1]);
			magic::cast_spell(ch, known_spells[i - 1], [=]() {
				game::add_action_seq([=]() {
					ch->ap -= 6;
					after_action();
				}, 0);
			});
		});
	}
	else
	{
		log(" This character cannot cast any spells!");
	}
}

void set_next_character_index()
{
	int ind = -1;
	for (int i = 1; i < COMBAT_MAX_CHARACTERS; i++)
	{
		int chk_ind = (i + combat::current_character_index) % COMBAT_MAX_CHARACTERS;
		Character* ch = game::active_characters[chk_ind];
		if (ch != NULL && !ch->is_dead())
		{
			ind = chk_ind;
			if (chk_ind <= combat::current_character_index)
				combat::turn_number++;
			break;
		}
	}
	if (ind == -1)
	{
		ind = combat::current_character_index;
	}
	combat::current_character_index = ind;
}

void after_action()
{
	Character* ch = game::active_characters[current_character_index];
	if (ch->ap <= 0)
	{
		game::add_action_seq([]() {
			after_turn();
		}, 0);
	}
	else
	{
		game::set_camera(ch->x, ch->y);
	}
}

void after_turn()
{
	combat::check_health();
	for (StatusEffect* se : status_effects)
	{
		if (se->ch == game::active_characters[current_character_index])
		{
			se->on_turn_end();
		}
	}

	combat::set_next_character_index();

	if (!combat::do_players_remain())
	{
		log("Your party has been eliminated.");
		game::game_over();
		return;
	}

	if (combat::do_enemies_remain())
	{
		Character* ch = game::active_characters[current_character_index];
		log("Now acting: " + ch->name);
		game::set_camera(ch->x, ch->y);

		ch->reset_ap();

		for (unsigned int i = 0; i < status_effects.size(); i++)
		{
			StatusEffect* se = status_effects[i];
			if (se->ch == ch)
			{
				if (se->should_remove)
				{
					status_effects.erase(status_effects.begin() + i);
					delete se;
					i--;
				}
				else
				{
					se->on_turn_begin();
				}
			}
		}

		if (ch->is_dead())
		{
			combat::check_health();
			after_turn();
			return;
		}

		combat::ai_acting = false;
		if (ch->is_cpu)
		{
			game::autodraw_enabled = true;
			game::disable_input();
			ch->set_sprite(" ", "cyan", "cyan");
			game::add_action_seq([=]() {
				ch->reset_sprite();
				combat::ai_acting = true;
				ai::do_behavior(ch);
			}, 3);
		}
		else
		{
			std::vector<Character*> party;
			player::load_party(party);
			for (unsigned int i = 0; i < party.size(); i++)
			{
				Character* c = party[i];
				if (c == ch)
				{
					player::select_character(i);
					break;
				}
			}

			ch->set_sprite(" ", "cyan", "cyan");
			game::autodraw_enabled = false;
			game::disable_input();
			game::add_action_seq([=]() {
				game::enable_input();
				ch->reset_sprite();
			}, 1);
		}
	}
	else
	{
		log("Combat completed!");
		game::wait_for_enter([]() {
			game::set_mode("explore");
		});
	}
}

Character* get_active_character()
{
	Character* ch = game::active_characters[current_character_index];
	return ch;
}

void check_health()
{
	for (int i = 0; i < COMBAT_MAX_CHARACTERS; i++)
	{
		Character* ch = game::active_characters[i];
		if (ch == NULL)
		{
			continue;
		}
		if (ch->is_dead())
		{
			log(ch->name + " has been eliminated!");
			game::active_characters[i] = NULL;
			for (unsigned int i = 0; i < status_effects.size(); i++)
			{
				StatusEffect* se = status_effects[i];
				if (se->ch == game::active_characters[current_character_index])
				{
					delete se;
					status_effects.erase(status_effects.begin() + i--);
				}
	}
		}
	}
}

bool do_enemies_remain()
{
	for (int i = 0; i < COMBAT_MAX_CHARACTERS; i++)
	{
		Character* ch = game::active_characters[i];
		if (ch == NULL)
		{
			continue;
		}
		if (ch->is_cpu)
		{
			return true;
		}
	}
	return false;
}

bool do_players_remain()
{
	for (int i = 0; i < COMBAT_MAX_CHARACTERS; i++)
	{
		Character* ch = game::active_characters[i];
		if (ch == NULL)
		{
			continue;
		}
		if (!ch->is_cpu)
		{
			return true;
		}
	}
	return false;
}

void get_adjacent_enemies(Character* ch, std::vector<Character*>& out)
{
	for (int x = -1; x <= 1; x++)
	{
		for (int y = -1; y <= 1; y++)
		{
			if (x == 0 && y == 0)
			{
				continue;
			}
			Character* ch2 = game::get_character_at(ch->x + x, ch->y + y);
			if (ch2 != NULL && ch2->is_cpu != ch->is_cpu)
			{
				out.push_back(ch2);
			}
		}
	}
}

void handle_keypress(char key)
{
	if (combat::ai_acting)
	{
		return;
	}

	int code = (int)key;
	Character* ch = game::active_characters[combat::current_character_index];
	bool did_action = false;
	if (code == 72 || code == 56) // up
	{
		did_action = combat::move_character(ch, 0, -1);
	}
	else if (code == 80 || code == 50) // down
	{
		did_action = combat::move_character(ch, 0, 1);
	}
	else if (code == 75 || code == 52) // left
	{
		did_action = combat::move_character(ch, -1, 0);
	}
	else if (code == 77 || code == 54) // right
	{
		did_action = combat::move_character(ch, 1, 0);
	}
	else if (code == 55) // nw (num7)
	{
		did_action = combat::move_character(ch, -1, -1);
	}
	else if (code == 57) // ne (num9)
	{
		did_action = combat::move_character(ch, 1, -1);
	}
	else if (code == 51) // se (num3)
	{
		did_action = combat::move_character(ch, 1, 1);
	}
	else if (code == 49) // sw (num1)
	{
		did_action = combat::move_character(ch, -1, 1);
	}
	else if (code == 53) // wait (num5)
	{
		did_action = true;
		ch->ap = 0;
		log(" Wait.");
	}
	else if (code == 32) // space
	{
		did_action = true;
		ch->ap = 0;
		log(" Wait.");
	}
	else if (key == 'm')
	{
		choose_magic();
		did_action = false;
	}
	else if (code == 'p') //for 'party'
	{
		player::log_player();
	}
	else if (code == 's')
	{
		player::log_storage();
	}
	else if (code == 'i') // for 'inventory'
	{
		player::show_inventory_menu();
	}
	else if (code == 'c')
	{
		player::log_character(player::get_selected_character());
	}

	if (did_action)
	{
		after_action();
	}
}

void update()
{
	if (game::running && combat::ai_acting && game::action_queue_seq.size() == 0)
	{
		Character* ch = game::active_characters[current_character_index];
		ai::do_behavior(ch);
	}
}
}