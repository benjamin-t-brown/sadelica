#include "Encounters.h"
#include "AI.h"

#include "Content/Content.h"

#include <map>
#include <string>

Encounter::Encounter(Encounter* enc)
{
	name = enc->name;
	x = enc->x;
	y = enc->y;
	behavior = enc->behavior;
	world_x = enc->world_x;
	world_y = enc->world_y;
}

std::string Encounter::save()
{
	std::stringstream ss;
	ss << x << "," << y << "," << world_x << "," << world_y << "," << behavior;
	return ss.str();
}

void move_encounter(Encounter* ch, int x_offset, int y_offset)
{
	int new_x = ch->x + x_offset;
	int new_y = ch->y + y_offset;

	Character* ch2 = game::get_character_at(new_x, new_y);

	int old_x = ch->x;
	int old_y = ch->y;
	ch->x += x_offset;
	ch->y += y_offset;

	if (ch2 != NULL)
	{
		ch->x = old_x;
		ch->y = old_y;
	}
	else if (ch->x < 0)
	{
		ch->x = 0;
	}
	else if (ch->x >= GAME_XMAPSIZE)
	{
		ch->x = GAME_XMAPSIZE - 1;
	}
	else if (ch->y < 0)
	{
		ch->y = 0;
	}
	else if (ch->y >= GAME_YMAPSIZE)
	{
		ch->y = GAME_YMAPSIZE - 1;
	}
	else if (game::map[ch->y][ch->x] >= GAME_MOVE_THRESHOLD || game::map[ch->y][ch->x] < 0)
	{
		ch->x = old_x;
		ch->y = old_y;
	}

	Character* pl = game::get_character_nearby(ch->x, ch->y, 1);

	if (pl != NULL && !pl->is_cpu)
	{
		log_dialog("Enemies attack!");
		game::wait_for_enter([=]() {
			encounters::initiate_encounter(ch);
		});
		return;
	}
}

namespace encounters
{
bool disable_encounters = false;
std::vector<std::shared_ptr<Encounter>> NO_ENCOUNTERS;
int world_x = 0;
int world_y = 0;
Character* enemies[GAME_MAX_CHARACTERS];
std::map<std::string, std::function<void()>> stored_encounters;
std::map<std::string, std::vector<std::shared_ptr<Encounter>>> active_encounters;

std::vector<std::shared_ptr<Encounter>>& get_encounters_at(int world_x, int world_y)
{
	if (disable_encounters)
		return NO_ENCOUNTERS;

	std::stringstream ss;
	ss << world_x << "," << world_y;
	std::string key = ss.str();
	auto it = active_encounters.find(key);
	if (it != active_encounters.end())
	{
		auto& encs = active_encounters[key];
		return encs;
	}
	else
	{
		return NO_ENCOUNTERS;
	}
}

void add_encounter(std::string key, std::function<void()> cb)
{
	stored_encounters[key] = cb;
}

void create_encounter(std::string name, int world_x, int world_y, int x, int y, std::string behavior)
{
	Encounter* enc = new Encounter(name, world_x, world_y, x, y, behavior);
	add_encounter_to_map(enc);
}

void add_encounter_to_map(Encounter* enc)
{
	std::stringstream ss;
	ss << enc->world_x << "," << enc->world_y;
	std::string key = ss.str();
	auto it = active_encounters.find(key);
	if (it != active_encounters.end())
	{
		std::vector<std::shared_ptr<Encounter>>& encs = active_encounters[key];
		encs.push_back(std::shared_ptr<Encounter>(enc));
	}
	else
	{
		std::vector<std::shared_ptr<Encounter>> encs;
		encs.push_back(std::shared_ptr<Encounter>(enc));
		active_encounters[key] = encs;
	}
}

void remove_encounter_from_map(Encounter* enc)
{
	std::vector<std::shared_ptr<Encounter>>& encs = get_encounters_at(world_x, world_y);
	for (unsigned int i = 0; i < encs.size(); i++)
	{
		Encounter* e = encs[i].get();
		if (e == enc)
		{
			encs.erase(encs.begin()+i);
			break;
		}
	}
}

void assign_enemies(Character* enemies[ENCOUNTER_MAX_ENEMIES])
{
	for (int i = 0; i < ENCOUNTER_MAX_ENEMIES; i++)
	{
		encounters::enemies[i] = enemies[i];
	}
}

void clear_character_array(Character* arr[])
{
	for (int i = 0; i < ENCOUNTER_MAX_ENEMIES; i++)
	{
		arr[i] = NULL;
	}
}

void initiate_encounter(Encounter* enc)
{
	game::encounter_name = enc->name;
	encounters::world_x = enc->world_x;
	encounters::world_y = enc->world_y;
	encounters::remove_encounter_from_map(enc);
	game::set_mode("combat");
}

Encounter* get_encounter_at(int world_x, int world_y, int x, int y)
{
	std::vector<std::shared_ptr<Encounter>>& encs = get_encounters_at(world_x, world_y);
	for (std::shared_ptr<Encounter> e : encs)
	{
		if (e->x == x && e->y == y)
		{
			return e.get();
		}
	}
	return NULL;
}

Encounter* get_encounter_nearby(Character* ch, int n)
{
	std::vector<std::pair<int,int>> offsets = {
		std::make_pair(-1, -1),
		std::make_pair(-1, 0),
		std::make_pair(-1, 1),
		std::make_pair(0, -1),
		std::make_pair(0, 1),
		std::make_pair(1, -1),
		std::make_pair(1, 0),
		std::make_pair(1, 1),
	};

	for (auto& offset: offsets)
	{
		int sy = ch->y + offset.first;
		int sx = ch->x + offset.second;
		for (int i = 0; i < n; i++ )
		{
			int y = sy + offset.first * i;
			int x = sx + offset.second * i;
			if(world::blocks_sight(x, y, game::map))
			{
				break;
			}
			if (y >= 0 && x >= 0 && y < GAME_YMAPSIZE && x < GAME_XMAPSIZE)
			{
				Encounter* enc = get_encounter_at(ch->world_x, ch->world_y, x, y);
				if(enc != NULL)
				{
					return enc;
				}
			}
		}
	}
	return NULL;
}

void set_encounter(std::string name)
{
	if (stored_encounters.find(name) != stored_encounters.end())
	{
		stored_encounters[name]();
	}
	else
	{
		throw std::runtime_error("Cannot find encounter with name '" + name + "'");
	}
}

bool print_encounter(int x, int y)
{
	Encounter* enc = get_encounter_at(game::current_world_x, game::current_world_y, x, y);
	if (enc != NULL)
	{
		draw_text(get_text("E", "white", "red"));
		return true;
	}
	return false;
}

void update_encounters()
{
	auto& encs = get_encounters_at(game::current_world_x, game::current_world_y);
	for (auto enc :encs)
	{
		if (enc->behavior == "random")
		{
			int arr[2];
			Character* pl = game::get_character_nearby(enc->x, enc->y, 4);
			if (pl != NULL && !pl->is_cpu)
			{
				ai::get_offset_towards(arr[0], arr[1], enc->x, enc->y, pl->x, pl->y);
			}
			else
			{
				std::string dir;
				int r = game::get_random(0, 4);
				dir = (r == 0 ? "n" : (r == 1 ? "s" : (r == 2 ? "e" : "w")));
				game::direction_to_offset(dir, arr);
			}
			move_encounter(enc.get(), arr[0], arr[1]);
		}
	}
}
}

void build_encounters()
{
	encounters::disable_encounters = false;
	std::vector<std::shared_ptr<Encounter>> NO_ENCOUNTERS;
	encounters::world_x = 0;
	encounters::world_y = 0;
	encounters::stored_encounters.clear();
	encounters::active_encounters.clear();
	content::build_encounters();
}
