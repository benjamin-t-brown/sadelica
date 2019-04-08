#include "Characters.h"
#include "Explore.h"
#include "Game.h"
#include "Items.h"
#include "Player.h"
#include "World.h"
#include "Combat.h"

#include "Content/Content.h"

std::shared_ptr<Character> character_list[CHARACTER_LIST_LENGTH];

void load_characters(int world_x, int world_y, Character* arr[GAME_MAX_CHARACTERS])
{
	for (int i = 0; i < GAME_MAX_CHARACTERS; i++)
	{
		arr[i] = NULL;
	}

	arr[0] = player::get_leader();
	int ctr = 1;
	for (int i = 1; i < CHARACTER_LIST_LENGTH; i++)
	{
		auto ch = character_list[i];
		if (ch != NULL)
		{
			if (ch->world_x == world_x && ch->world_y == world_y)
			{
				arr[ctr] = ch.get();
				ctr++;
			}
		}
	}
}

Character* get_character(int i)
{
	return character_list[i].get();
}

Character* get_character(std::string name)
{
	for (int i = 1; i < CHARACTER_LIST_LENGTH; i++)
	{
		auto ch = character_list[i];
		if (ch != NULL)
		{
			if (ch->name == name)
			{
				return ch.get();
			}
		}
	}
	return NULL;	
}

Character::Character(std::string name, std::string text, std::string color, std::string bgcolor, int world_x, int world_y, int x, int y, std::string behavior)
{
	init(name, text, color, bgcolor, world_x, world_y, x, y, behavior);
}

Character::Character(std::string name, std::string text, std::string color, std::string bgcolor, int world_x, int world_y, int x, int y)
{
	init(name, text, color, bgcolor, world_x, world_y, x, y, "none");
}


Character::Character()
{
	init("Unnamed", "?", "black", "white", 0, 0, 0, 0, "none");
}

void Character::init(std::string name, std::string text, std::string color, std::string bgcolor, int world_x, int world_y, int x, int y, std::string behavior)
{
	this->name = name;
	this->stored_coords = { 0, 0, 0, 0 };
	this->old_x = 0;
	this->old_y = 0;
	this->x = x;
	this->y = y;
	this->world_x = world_x;
	this->world_y = world_y;
	this->text = text;
	this->color = color;
	this->bgcolor = bgcolor;
	this->behavior = behavior;
	this->is_cpu = true;
	this->base_talk_index = 0;
	this->talk_index = 0;
	this->ap = COMBAT_BASE_AP;
	this->max_ap = 4;
	this->ap = 4;
	this->max_hp = 10;
	this->hp = 10;
	this->armor_class = 12;
	this->bonus_armor_class = 0;
	this->attack = 5;
	this->bonus_attack = 0;
	this->max_damage = 8;
	this->bonus_max_damage = 0;
	this->dmg_variance_percent = 100;
	this->magic_dmg_trust_deviation = 5;
	this->magic = 1;
	this->bonus_magic = 0;
	this->damage_reduction = 0;
	this->bonus_damage_reduction = 0;
	this->effect_damage_reduction = 0;
	this->old_color = this->color;
	this->old_bgcolor = this->bgcolor;
	this->old_text = this->text;
}

int Character::get_talk_index(std::function<void(Character*)> func)
{
	size_t addr = get_pointer_address(func);
	for(unsigned int i = 0; i < dialogs.size(); i++)
	{
		if (get_pointer_address(dialogs[i]) == addr)
		{
			return i;
		}
	}
	return 0;
}

void Character::print()
{
	draw_text(get_text(text, color, bgcolor));
}

void Character::act()
{
	if (behavior == "random")
	{
		int arr[2];
		std::string dir;
		int r = game::get_random(0, 4);
		dir = (r == 0 ? "n" : (r == 1 ? "s" : (r == 2 ? "e" : "w")));
		game::direction_to_offset(dir, arr);
		explore::move_npc(this, arr[0], arr[1]);
	}
}
void Character::store_coords()
{
	stored_coords = get();
}
void Character::restore_coords()
{
	set(stored_coords[0], stored_coords[1], stored_coords[2], stored_coords[3]);
}
std::vector<int> Character::get()
{
	std::vector<int> coords;
	coords.push_back(world_x);
	coords.push_back(world_y);
	coords.push_back(x);
	coords.push_back(y);
	return coords;
}
void Character::set(int wx, int wy, int lx, int ly)
{
	world_x = wx;
	world_y = wy;
	x = lx;
	y = ly;
}

std::string Character::get_str_position()
{
	return std::to_string(world_x) + "," + std::to_string(world_y) + "," + std::to_string(x) + "," + std::to_string(y);
}

void Character::hide()
{
	set(0, 0, 0, 0);
}

void Character::reset_sprite()
{
	this->color = this->old_color;
	this->bgcolor = this->old_bgcolor;
	this->text = this->old_text;
}

void Character::set_sprite(std::string text, std::string color, std::string bgcolor)
{
	this->text = text;
	this->color = color;
	this->bgcolor = bgcolor;
}

void Character::reset_ap()
{
	ap = max_ap;
}
void Character::reset_hp()
{
	hp = max_hp;
}
void Character::modify_hp(int n)
{
	if (is_cpu)
	{
		hp += n;
		return;
	}

	if (hp == -1)
	{
		return;
	}

	hp += n;
	if (hp > max_hp)
	{
		hp = max_hp;
	}
	else if (hp < 0)
	{
		if (hp - n == 0)
		{
			hp = -1;
		}
		else
		{
			hp = 0;
		}
	}
}
bool Character::is_dead()
{
	return hp <= -1;
}
bool Character::is_on_current_map()
{
	if (world_x == game::current_world_x && world_y == game::current_world_y)
	{
		return true;
	}
	return false;
}

void Character::set_talk_index(int i)
{
	talk_index = i;
}
void Character::set_talk_index(std::string str)
{
	if (dialog_mapping.find(str) != dialog_mapping.end())
	{
		talk_index = dialog_mapping[str];
	}
	else
	{
		log("WARNING: No dialog index named '" + str + "' found in character: '" + name + "'");
	}
}

void Character::set_base_talk_index(int i)
{
	base_talk_index = i;
}
void Character::set_base_talk_index(std::string str)
{
	if (dialog_mapping.find(str) != dialog_mapping.end())
	{
		base_talk_index = dialog_mapping[str];
	}
	else
	{
		log("WARNING: No dialog index named '" + str + "' found in character: '" + name + "'");
	}
}

void Character::add_dialog(std::function<void(Character*)> cb, std::string label)
{
	dialogs.push_back(cb);
	if (label.size())
	{
		dialog_mapping[label] = dialogs.size() - 1;
	}
}

std::string Character::save()
{
	std::stringstream ss;
	ss << x << "," << y << "," << world_x << "," << world_y;
	return ss.str();
}

std::string Character::save_player_character()
{
	std::stringstream ss;
	ss << x << "," << y << "," << world_x << "," << world_y << ",";
	ss << max_ap << ",";
	ss << ap << ",";
	ss << max_hp << ",";
	ss << hp << ",";
	ss << armor_class << ",";
	ss << bonus_armor_class << ",";
	ss << attack << ",";
	ss << bonus_attack << ",";
	ss << max_damage << ",";
	ss << bonus_max_damage << ",";
	ss << dmg_variance_percent << ",";
	ss << magic << ",";
	ss << bonus_magic << ",";
	ss << damage_reduction << ",";
	ss << bonus_damage_reduction << ",";
	ss << effect_damage_reduction << ",";
	for (auto& item_name: items)
	{
		ss << item_name << ":";
	}
	ss << ",";
	for (auto& spell_name: spells)
	{
		ss << spell_name << ":";
	}
	ss << "," << name;

	return ss.str();
}

namespace characters
{

void show_store(std::string store_name)
{
	std::vector<std::pair<std::string, int>> store;
	load_store(store_name, store);
	std::vector<std::string> choices;
	for (int i = 0; i < (int)store.size(); i++)
	{
		Item* item = get_item(store[i].first);
		if (item != NULL)
		{
			std::stringstream ss;
			ss << item->name << " - " << item->description << " | Cost: " << store[i].second << " coins";
			choices.push_back(ss.str());
		}
	}
	choices.push_back("Leave.");
	game::wait_for_choice(store_name + ":", choices, [=](int i) {
		if (i > 0 && i < (int)choices.size() - 1)
		{
			std::string s = store_name;
			std::vector<std::string> choices2 = {"Yes", "No"};
			std::string item_name = store[i - 1].first;
			game::wait_for_choice("Are you sure you want to buy this item: " + item_name + "?", choices2, [=](int i) {
				if (i > 0)
				{
					if (i == 1)
					{
						log("You bought: " + item_name);
					}
					else if (i == 2)
					{
						show_store(s);
					}
				}
			});
		}
	});
}

void show_sell()
{
	log("Not implemented (show_sell)");
}

void add_character(Character* ch)
{
	for (int i = 0; i < CHARACTER_LIST_LENGTH; i++)
	{
		if (character_list[i] == NULL)
		{
			character_list[i] = std::shared_ptr<Character>(ch);
			break;
		}
	}
}

void ultimatum(std::string text, Character* ch, std::function<void()> yes_cb, std::function<void()> no_cb)
{
	std::vector<std::string> choices = {
		"Yes.",
		"No."};
	game::wait_for_choice(text, choices, [=](int i) {
		if (i == 1)
		{
			yes_cb();
		}
		else if (i == 2)
		{
			no_cb();
		}
		else
		{
			ultimatum(text, ch, yes_cb, no_cb);
		}
	});
	explore::end_talk(ch);
}
}

void build_characters()
{
	for (int i = 0; i < CHARACTER_LIST_LENGTH; i++)
	{
		character_list[i] = NULL;
	}

	character_list[0] = std::shared_ptr<Character>(new Character("Player1", "1", "b_blue", "b_white", TO4D(70,230)));
	character_list[0]->is_cpu = false;
	character_list[0]->hp = 15;
	character_list[0]->max_hp = 20;
	character_list[0]->armor_class = 8;
	character_list[0]->attack = 5;
	character_list[0]->dmg_variance_percent = 50;
	character_list[0]->max_damage = 5;
	character_list[1] = std::shared_ptr<Character>(new Character("Player2", "2", "b_blue", "b_white", -1, -1, 0, 0));
	character_list[1]->is_cpu = false;
	character_list[1]->hp = 15;
	character_list[1]->max_hp = 20;
	character_list[1]->armor_class = 8;
	character_list[1]->attack = 5;
	character_list[1]->dmg_variance_percent = 50;
	character_list[1]->max_damage = 5;
	character_list[2] = std::shared_ptr<Character>(new Character("Player3", "3", "b_blue", "b_white", -1, -1, 0, 0));
	character_list[2]->is_cpu = false;
	character_list[2]->hp = 15;
	character_list[2]->max_hp = 20;
	character_list[2]->armor_class = 8;
	character_list[2]->attack = 5;
	character_list[2]->dmg_variance_percent = 50;
	character_list[2]->max_damage = 5;
	character_list[3] = std::shared_ptr<Character>(new Character("Player4", "4", "b_blue", "b_white", -1, -1, 0, 0));
	character_list[3]->is_cpu = false;
	character_list[3]->hp = 15;
	character_list[3]->max_hp = 20;
	character_list[3]->armor_class = 8;
	character_list[3]->attack = 5;
	character_list[3]->dmg_variance_percent = 50;
	character_list[3]->max_damage = 5;
	character_list[4] = std::shared_ptr<Character>(new Character("Player5", "5", "b_blue", "b_white", -1, -1, 0, 0));
	character_list[4]->is_cpu = false;
	character_list[4]->hp = 15;
	character_list[4]->max_hp = 20;
	character_list[4]->armor_class = 8;
	character_list[4]->attack = 5;
	character_list[4]->dmg_variance_percent = 50;
	character_list[4]->max_damage = 5;

	content::build_characters();

	player::move_party_members();

	// debug for starting with dead characters
	//character_list[1]->hp = -1;
	//character_list[2]->hp = -1;
	//character_list[3]->hp = -1;
	//character_list[4]->hp = -1;
}
