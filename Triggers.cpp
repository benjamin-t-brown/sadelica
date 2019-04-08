#include "Triggers.h"
#include "DrawText.h"
#include "Encounters.h"
#include "Explore.h"
#include "Game.h"
#include "Items.h"
#include "Player.h"
#include "World.h"

#include "Content/Content.h"

#include <functional>

std::string get_heading(Character* ch);

void _empty_trigger(Character* ch)
{
	log_dialog("WARNING! This trigger has no definition!");
}

void build_signs()
{
	content::build_signs();
}

void build_triggers()
{
	triggers::trigger_list.clear();
	content::build_triggers();
}

Sign::Sign(std::string text, int x, int y, int i, int j)
{
	this->text = text;
	std::stringstream ss;
	ss << x << "," << y << "," << i << "," << j;
	triggers::signs[ss.str()] = std::shared_ptr<Sign>(this);
}

Chest::Chest(std::string names, int x, int y, int i, int j)
{
	this->x = x;
	this->y = y;
	this->i = i;
	this->j = j;
	strutil::split(names, ",", item_names);
	std::stringstream ss;
	ss << x << "," << y << "," << i << "," << j;
	triggers::chests[ss.str()] = std::shared_ptr<Chest>(this);
}

std::string Chest::save()
{
	std::stringstream ss;
	ss << x << "," << y << "," << i << "," << j << "=";
	for (std::string& item_name: item_names)
	{
		ss << item_name << ",";
	}
	return ss.str();
}

std::string get_sign_text(int x, int y, int i, int j)
{
	std::stringstream ss;
	ss << x << "," << y << "," << i << "," << j;
	if (triggers::signs.find(ss.str()) != triggers::signs.end())
	{
		return triggers::signs[ss.str()]->text;
	}
	else
	{
		return "";
	}
}

bool show_chest(int x, int y, int ii, int jj)
{
	std::stringstream ss;
	ss << x << "," << y << "," << ii << "," << jj;

	if (triggers::chests.find(ss.str()) != triggers::chests.end())
	{
		auto chest = triggers::chests[ss.str()];
		if (chest->item_names.size() == 0)
		{
			return false;
		}
		std::vector<std::string> choices = chest->item_names;
		choices.push_back("Nevermind");
		game::allow_esc_while_waiting();
		game::wait_for_choice("You find these things:", choices, [=](int i){
			if (i == (int)choices.size() || i == 0)
			{
				return;
			}
			std::string item_name = chest->item_names[i-1];
			Item* item = get_item(item_name);
			if (item == NULL)
			{
				return;
			}
			if (!item->ephemeral)
				log(" You put this " + item_name + " in your backpack");
			player::store_item(item_name);
			chest->item_names.erase(chest->item_names.begin()+i-1);
			if (chest->item_names.size())
				show_chest(x, y, ii, jj);
			else
				log( " Chest is now empty.");
		});
		return true;
	}
	else
	{
		return false;
	}
}

bool call_trigger(int x, int y, int i, int j, Character* ch)
{
	std::stringstream ss;
	ss << x << "," << y << "," << i << "," << j;
	std::string id = ss.str();
	if (world::trigger_locations.find(id) != world::trigger_locations.end())
	{
		apply_trigger(world::trigger_locations[id], ch);
		return true;
	}
	return false;
}

void apply_trigger(std::string id, Character* ch)
{
	if (triggers::trigger_list.find(id) != triggers::trigger_list.end())
	{
		auto cb = triggers::trigger_list[id];
		cb(ch);
	}
	else
	{
		log("WARNING Cannot call trigger with id " + id);
	}
}

namespace triggers
{
std::map<std::string, std::shared_ptr<Sign>> signs;
std::map<std::string, std::shared_ptr<Chest>> chests;
//std::function<void(Character*)> trigger_list[TRIGGER_LIST_LENGTH];
std::map<std::string, std::function<void(Character*)>> trigger_list;

bool once(std::string key)
{
	if (player::has(key))
		return false;

	player::set(key);
	return true;
}
bool once_reset_tile(std::string key, int tile_id)
{
	if (player::has(key))
		return false;

	Character* ch = player::get_leader();
	game::set_tile(tile_id, ch->x, ch->y);
	player::set(key);
	return true;
}
}
