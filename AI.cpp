#include "AI.h"
#include "Combat.h"
#include "Game.h"
#include "Magic.h"
#include "Player.h"
#include "World.h"
#include "Characters.h"

#include <cmath>
#include <queue>

void move_randomly(Character* ch)
{
	int x_offset = 0;
	int y_offset = 0;
	if (ch->x < COMBAT_XMAPSIZE / 2)
	{
		x_offset = 1;
	}
	else if (ch->x > COMBAT_XMAPSIZE / 2)
	{
		x_offset = -1;
	}
	else if (ch->y > COMBAT_YMAPSIZE / 2)
	{
		y_offset = -1;
	}
	else if (ch->y < COMBAT_YMAPSIZE / 2)
	{
		y_offset = 1;
	}

	if (!combat::move_character(ch, x_offset, y_offset))
	{
		ch->ap--;
	}
}

void move_badly(Character* ch)
{
	Character* enemy = ai::get_nearest_enemy(ch);
	if (enemy != NULL)
	{
		static std::string dirs[8] = {"n", "ne", "e", "se", "s", "sw", "w", "nw"};
		int x_offset = 0, y_offset = 0;
		ai::get_offset_towards(x_offset, y_offset, ch, enemy);

		std::string dir = game::offset_to_direction(x_offset, y_offset);
		int dir_ind = 0;
		for (int i = 0; i < 8; i++)
		{
			if (dirs[i] == dir)
			{
				dir_ind = i;
			}
		}

		std::string dir_left = dirs[(dir_ind - 1 + 8) % 8];
		std::string dir_right = dirs[(dir_ind + 1 + 8) % 8];
		int x_offset_left = x_offset, y_offset_left = y_offset;
		game::direction_to_offset(dir_left, x_offset_left, y_offset_left);
		int x_offset_right = x_offset, y_offset_right = y_offset;
		game::direction_to_offset(dir_right, x_offset_right, y_offset_right);
		if (!combat::move_character(ch, x_offset, y_offset))
		{
			if (!combat::move_character(ch, x_offset_left, y_offset_left))
			{
				ch->ap--;
			}
			else if (!combat::move_character(ch, x_offset_right, y_offset_right))
			{
				ch->ap--;
			}
			else
			{
				ch->ap--;
			}
		}
	}
	else
	{
		ch->ap = 0;
	}
}

void move_aggressively(Character* ch)
{
	Character* enemy = ai::get_nearest_enemy(ch);
	if (enemy != NULL)
	{
		std::pair<int,int> offset = ai::get_path_next(ch->x, ch->y, enemy->x, enemy->y, game::map);
		if (!combat::move_character(ch, offset.first, offset.second))
		{
			ch->ap = 0;
		}
	}
}

namespace ai
{
void do_behavior(Character* ch)
{
	if (ch->behavior == "move")
	{
		move_randomly(ch);
	}
	else if (ch->behavior == "aggressive")
	{
		move_aggressively(ch);
	}
	else if (ch->behavior == "supportive_caster_lvl1")
	{
		if (game::get_random(1, 10) > 5)
		{
			std::string spell_name;
			if (game::get_random(1, 10) > 4)
			{
				spell_name = "Scorch";
				magic::set_cpu_targeting_func([=]() {
					std::vector<Character*> ret;
					ret.push_back(get_random_player());
					if (ret.size()) game::set_camera(ret[0]);
					return ret;
				});
			}
			else
			{
				spell_name = game::pick_string({"Light Haste", "Light Bless"});
				magic::set_cpu_targeting_func([=]() {
					std::vector<Character*> ret;
					ret.push_back(get_random_cpu());
					if (ret.size()) game::set_camera(ret[0]);
					return ret;
				});
			}

			magic::cast_spell(ch, spell_name, [=]() {
				game::add_action_seq([=]() {
					ch->ap -= 6;
				},0);
			});
		}
		else
		{
			move_badly(ch);
			ch->ap--;
		}
	}
	else if (ch->behavior == "aggressive_caster_lvl1")
	{
	}
	else if (ch->behavior == "haste_attack")
	{
		static std::vector<std::string> spells = {
			"Move",
			"Haste Self",
			"Move",
			"Move"
		};
		std::string spell_name = spells[combat::turn_number % spells.size()];
		if (spell_name == "Move")
		{
			move_aggressively(ch);
		}
		else
		{
			magic::cast_spell(ch, spell_name, [=]() {
				game::add_action_seq([=]() {
					ch->ap -= 6;
				},0);
			});
		}
	}
	else if (ch->behavior == "exploder")
	{
		std::vector<Character*> adjacents;
		combat::get_adjacent_enemies(ch, adjacents);
		if (adjacents.size() >= 2)
		{
			magic::cast_spell(ch, "Explode", [=]() {
				game::add_action_seq([=]() {
					ch->ap -= 6;
					ch->hp = -1;
				},0);
			});
		}
		else
		{
			move_aggressively(ch);
		}
	}
	else if (ch->behavior == "graulark")
	{
		if (game::get_random(1, 10) > 5)
		{
			magic::set_cpu_targeting_func([=]() {
				std::vector<Character*> ret;
				ret.push_back(get_random_player());
				if (ret.size()) game::set_camera(ret[0]);
				return ret;
			});
			magic::cast_spell(ch, "Long Claw", [=]() {
				game::add_action_seq([=]() {
					ch->ap -= 6;
				},0);
			});
		}
		else
		{
			log(" The Groulark Claw flails about!");
			game::add_action_seq([=]() {
				ch->ap -= 6;
			}, 0);
		}
		ch->ap = 0;
	}
	else if (ch->behavior == "graulark_head")
	{
		magic::set_cpu_targeting_func([=]() {
			std::vector<Character*> ret;
			ret.push_back(get_random_injured_cpu());
			game::set_camera(ret[0]);
			return ret;
		});
		magic::cast_spell(ch, "Weak Heal", [=]() {
			game::add_action_seq([=]() {
				ch->ap -= 6;
			}, 0);
		});
		ch->ap = 0;
	}
	else
	{
		ch->ap = 0;
	}

	game::add_action_seq([]() {
		combat::after_action();
	}, 0);
}

float get_distance_to(Character* ch_from, Character* ch_to)
{
	float vx = ch_to->x - ch_from->x;
	float vy = ch_to->y - ch_from->y;

	return sqrt(vx * vx + vy * vy);
}

void get_offset_towards(int& x, int& y, int x1, int y1, int x2, int y2)
{
	int vx = x2 - x1;
	int vy = y2 - y1;

	if (vx == 0)
		x = 0;
	else
		x = abs(vx) / vx;

	if (vy == 0)
		y = 0;
	else
		y = abs(vy) / vy;
}

void get_offset_towards(int& x, int& y, Character* ch_from, Character* ch_to)
{
	get_offset_towards(x, y, ch_from->x, ch_from->y, ch_to->x, ch_to->y);
}

Character* get_nearest_enemy(Character* ch)
{
	Character* ret = NULL;
	float lowest_dist = 9999999;
	for (int i = 0; i < GAME_MAX_CHARACTERS; i++)
	{
		Character* ch2 = game::active_characters[i];
		if (ch2 != NULL)
		{
			if (ch2->is_dead())
				continue;
			if (ch->is_cpu != ch2->is_cpu)
			{
				float dist = get_distance_to(ch, ch2);
				if (dist < lowest_dist)
				{
					lowest_dist = dist;
					ret = ch2;
				}
			}
		}
	}
	return ret;
}

Character* get_random_player()
{
	std::vector<Character*> party;
	player::load_alive_party(party);
	int i = game::get_random(0, party.size());
	return party[i];
}

Character* get_random_cpu()
{
	std::vector<Character*> list;
	for (int i = 0; i < GAME_MAX_CHARACTERS; i++)
	{
		Character* c = game::active_characters[i];
		if (c != NULL && c->is_cpu && !c->is_dead())
		{
			list.push_back(c);
		}
	}
	int i = game::get_random(0, list.size());
	return list[i];
}

Character* get_random_injured_cpu()
{
	std::vector<Character*> list;
	for (int i = 0; i < GAME_MAX_CHARACTERS; i++)
	{
		Character* c = game::active_characters[i];
		if (c != NULL && c->is_cpu && !c->is_dead() && c->hp < c->max_hp)
		{
			list.push_back(c);
		}
	}
	int i = game::get_random(0, list.size());
	return list[i];
}

struct PathNode {
	int value;
	int x;
	int y;
	bool visited;
	std::shared_ptr<PathNode> parent;
};

std::pair<int,int> get_path_next(int x1, int y1, int x2, int y2, int map[GAME_YMAPSIZE][GAME_XMAPSIZE])
{
	if (x1 == x2 && y1 == y2)
	{
		return std::make_pair(0, 0);
	}

	if (game::is_out_of_bounds(x1, y1) || game::is_out_of_bounds(x2, y2))
	{
		return std::make_pair(0, 0);
	}

	static const std::vector<std::pair<int,int>> adj_offsets = {
		{0, -1},
		{0, 1},
		{1, 0},
		{-1, 0},
		{1, 1},
		{-1, 1},
		{1, -1},
		{-1, -1}
	};
	std::queue<std::shared_ptr<PathNode>> Q;
	std::vector<std::shared_ptr<PathNode>> adj_matrix;
	bool is_found = false;

	for (int i = 0; i < GAME_YMAPSIZE; i++)
	{
		for (int j = 0; j < GAME_XMAPSIZE; j++)
		{
			auto node = std::make_shared<PathNode>();
			node->x = j;
			node->y = i;
			node->visited = false;
			node->parent = NULL;
			node->value = world::is_wall(map[i][j]) ? 0 : 1;
			adj_matrix.push_back(node);
		}
	}

	adj_matrix[TO1D(x1, y1)]->visited = true;
	Q.push(adj_matrix[TO1D(x1, y1)]);
	while (!Q.empty())
	{
		auto node = Q.front();
		Q.pop();
		if (node == adj_matrix[TO1D(x2, y2)])
		{
			is_found = true;
			break;
		}

		for (auto& offset: adj_offsets)
		{
			int nx = node->x + offset.first;
			int ny = node->y + offset.second;
			if (game::is_out_of_bounds(nx, ny))
				continue;

			auto child = adj_matrix[TO1D(nx, ny)];
			Character* ch = game::get_character_at(nx, ny);
			if (ch != NULL && ch->is_cpu)
				continue;

			if (!child->visited && child->value)
			{
				child->visited = true;
				child->parent = node;
				Q.push(child);
			}
		}
	}

	if (!is_found)
	{
		return std::make_pair(0, 0);
	}

	auto node = adj_matrix[TO1D(x2, y2)];
	auto node2 = adj_matrix[TO1D(x2, y2)];
	while (node->parent)
	{
		node2 = node;
		node = node->parent;
	}

	return std::make_pair(node2->x - node->x, node2->y - node->y);
}
}