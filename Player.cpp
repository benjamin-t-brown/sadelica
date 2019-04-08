#include "Player.h"
#include "Characters.h"
#include "Items.h"
#include "Combat.h"
#include "Explore.h"
#include "Magic.h"

#include <iomanip>
#include <map>

std::map<std::string, std::string> player_data;

void build_player()
{
	player_data.clear();
	player::item_storage.clear();
	player::quest_items.clear();
	player::selected_character = 0;
	player::max_inventory_slots = 6;

	player_data["coins"] = std::to_string(100);

	player::add_item("Cap", get_character(0));
	player::add_item("Leather Cuirass", get_character(0));
	player::add_item("Sword", get_character(0));
	player::add_item("All Fire", get_character(0));

	player::add_item("Sword", get_character(1));
	player::add_item("Thick Cloak", get_character(1));
	player::add_item("All Fire", get_character(1));

	player::add_item("Cap", get_character(2));
	player::add_item("Dagger", get_character(2));
	player::add_item("Fire Stone", get_character(2));

	player::add_item("Cap", get_character(3));
	player::add_item("Fire Stone", get_character(3));
	player::add_item("Dagger", get_character(3));

	player::store_item("Cap");
	player::store_item("Sword");
	player::store_item("Thick Cloak");
	player::store_item("Dagger");
	player::store_item("Dagger");

	player::store_item("Bandage");
	player::store_item("Flyleaf Mushroom");
	player::store_item("Bandage");
	player::store_item("Flyleaf Mushroom");
	player::store_item("Bandage");

	player::store_item("Healing Crystal");
	player::store_item("Healing Crystal");
	player::store_item("Healing Crystal");
	player::store_item("Healing Crystal");

	player::store_item("Revitalizer");

	player::store_item("Candle");
	player::store_item("Harkin Lance");
	player::store_item("Rock");
	player::store_item("Stick");
	player::store_item("Blank Paper");
	player::store_item("Inkpot");
	player::store_item("Aromatic Candle");

	player::store_item("Armor Olive");
	player::store_item("Bad Mead");

	player::store_item("Small Omni Flower");
	player::store_item("Small Omni Flower");
	player::store_item("Small Omni Flower");
	player::store_item("Small Omni Flower");

	player::store_item("Med Omni Flower");
	player::store_item("Med Omni Flower");
	player::store_item("Med Omni Flower");
	player::store_item("Med Omni Flower");

	player::store_item("Large Omni Flower");
	player::store_item("Large Omni Flower");
	player::store_item("Large Omni Flower");
	player::store_item("Large Omni Flower");

	Character* ch = player::get_selected_character();
	if (ch->is_dead())
		player::select_character(-1);
}

enum {
	STORAGE_FILTER_NONE,
	STORAGE_FILTER_USABLE,
	STORAGE_FILTER_USABLE_NOW
};

namespace player
{
std::vector<std::string> item_storage;
std::vector<std::string> quest_items;
int selected_character = 0;
int max_inventory_slots = 6;

std::string save_player_data()
{
	std::stringstream ss;
	for (auto it: player_data)
	{
		ss << it.first << "=" << it.second << ",";
	}
	return ss.str();
}
std::string save_item_storage()
{
	std::stringstream ss;
	for (auto& item_name: item_storage)
	{
		ss << item_name << ",";
	}
	return ss.str();
}
Character* get_leader()
{
	std::vector<Character*> party;
	load_alive_party(party);
	if (party.size() == 0)
		return NULL;
	return party[0];
}

std::map<std::string, std::string>& get_player_data()
{
	return player_data;
}

void load_party(std::vector<Character*>& party)
{
	for (int i = 0; i < 5; i++)
	{
		Character* ch = get_character(i);
		party.push_back(ch);
	}
}

void load_alive_party(std::vector<Character*>& party)
{
	for (int i = 0; i < 5; i++)
	{
		Character* ch = get_character(i);
		if (!ch->is_dead())
			party.push_back(ch);
	}
}

void load_spells(Character* ch, std::vector<std::string>& spells)
{
	for (std::string spell_name : ch->spells)
	{
		spells.push_back(spell_name);
	}
}

void load_inventory(Character* ch, std::vector<std::string>& inventory)
{
	for (std::string item_name : ch->items)
	{
		inventory.push_back(item_name);
	}
}

void move_party_members()
{
	Character* leader = get_leader();
	std::vector<Character*> party;
	load_party(party);
	for (Character* ch: party)
	{
		if (ch != leader)
		{
			ch->set(leader->world_x, leader->world_y, leader->x, leader->y);
		}
	}

}
void add_spell(std::string spell_name, Character* ch)
{
	for (int i = 0; i < (int)ch->spells.size(); i++)
	{
		if (ch->spells[i] == spell_name)
		{
			return;
		}
	}
	ch->spells.push_back(spell_name);
}
void remove_spell(std::string spell_name, Character* ch)
{
	for (int i = 0; i < (int)ch->spells.size(); i++)
	{
		if (ch->spells[i] == spell_name)
		{
			ch->spells.erase(ch->spells.begin() + i);
			return;
		}
	}
}
void store_item(std::string item_name, bool log_item)
{
	Item* item = get_item(item_name);
	if (item != NULL)
	{
		if (log_item)
		{
			log_dialog("");
			log_dialog("'" + item_name + "' added to backpack."); 
			log_dialog("");
		}
		if (item->ephemeral)
		{
			item->on_acquire_ephemeral();
			return;
		}

		item_storage.push_back(item_name);
		std::sort(item_storage.begin(), item_storage.end());
	}
}
void unstore_item(std::string item_name, bool log_item)
{
	auto iter = std::find(item_storage.begin(), item_storage.end(), item_name);
	if(iter != item_storage.end())
	{
		item_storage.erase(iter);
		if (log_item)
		{
			log_dialog("");
			log_dialog("'" + item_name + "' removed from backpack."); 
			log_dialog("");
		}
	}
}
bool add_item(std::string item_name, Character* ch)
{
	Item* item = get_item(item_name);
	if (item == NULL)
	{
		return false;
	}
	if (item->ephemeral)
	{
		store_item(item_name);
		return false;
	}
	if ((int)ch->items.size() >= player::max_inventory_slots)
	{
		return false;
	}
	ch->items.push_back(item_name);
	item->on_acquire(ch);
	return true;
}
bool remove_item(std::string item_name, Character* ch)
{
	for (int i = 0; i < (int)ch->items.size(); i++)
	{
		if (ch->items[i] == item_name)
		{
			ch->items.erase(ch->items.begin() + i);
			Item* item = get_item(item_name);
			if (item != NULL)
			{
				item->on_unacquire(ch);
				return true;
			}
		}
	}
	return false;
}
bool remove_item_ind(int ind, Character* ch)
{
	if (ind >= 0 && ind < (int)ch->items.size())
	{
		Item* item = get_item(ch->items[ind]);
		ch->items.erase(ch->items.begin() + ind);
		if (item != NULL)
		{
			item->on_unacquire(ch);
			return true;
		}
	}
	return false;
}

bool has_item(std::string item_name)
{
	std::vector<Character*> party;
	load_alive_party(party);
	for (int i = 0; i < (int)party.size(); i++)
	{
		Character* ch = party[i];
		std::vector<std::string> inventory;
		load_inventory(ch, inventory);
		for (std::string item_name2 : inventory)
		{
			if (item_name2 == item_name)
			{
				return true;
			}
		}
	}

	for (std::string& item_name2: item_storage)
	{
		if (item_name2 == item_name)
		{
			return true;
		}
	}

	return false;
}
void set_at(int x, int y, int i, int j)
{
	std::vector<Character*> party;
	load_party(party);
	party[0]->set(x, y, i, j);	
}

bool select_character(int i)
{
	int old_selected_character = player::selected_character;
	if (i >= 0 && i <= 4)
	{
		player::selected_character = i;
		Character* ch = get_selected_character();
		if (ch == NULL || ch->is_dead())
		{
			player::selected_character = old_selected_character;
			return false;
		}
		else
		{
			return true;
		}
	}
	if (i == -1)
	{
		int ctr = 1;
		int new_selected_character = (old_selected_character + 1) % 5;
		while (!select_character(new_selected_character))
		{
			ctr++;
			if (ctr == 5)
			{
				return false;
			}
			new_selected_character = (old_selected_character + ctr) % 5;
		}
		return true;
	}
	return false;
}

void transfer_item(Character* from, int ind, Character* to, int ind2)
{
	std::vector<std::string> inventory_from;
	load_inventory(from, inventory_from);

	std::vector<std::string> inventory_to;
	load_inventory(to, inventory_to);

	std::string item_name_from = inventory_from[ind];
	std::string item_name_to = "";
	if (ind2 >= 0 && ind2 < (int)inventory_to.size())
	{
		item_name_to = "";
		remove_item_ind(ind2, to);
	}
	remove_item_ind(ind, from);
	add_item(item_name_from, to);
	remove_item(item_name_to, from);
}

void modify_coins(int v)
{
	std::string coins = player_data["coins"];
	int c = std::stoi(coins);
	c += v;
	player_data["coins"] = std::to_string(c);
	if (v > 0)
		log(" Got " + std::to_string(v) + " coins.");
	else
		log(" Lost " + std::to_string(v) + " coins.");
}

void set(std::string key, std::string val)
{
	player_data[key] = val;
}
std::string get_string(std::string key)
{
	if (player_data.find(key) != player_data.end())
	{
		return player_data[key];
	}
	else
	{
		return "";
	}
}
bool get(std::string key)
{
	return (get_string(key) == "yes");
}
bool has(std::string key)
{
	return (get_string(key) == "yes");
}

int get_int(std::string key)
{
	int ret = -1;
	std::string str = get_string(key);
	if (str.size())
	{
		try
		{
			ret = std::stoi(str);
		}
		catch (std::exception& e)
		{
			log_debug("WARNING, invalid get_int with key '" + key + "'");
		}
	}
	return ret;
}
void set_int(std::string key, int v)
{
	set(key, std::to_string(v));
}

void add_quest_item(std::string item_name)
{
	quest_items.push_back(item_name);
}
bool remove_quest_item(std::string item_name)
{
	for (unsigned int i = 0; i < quest_items.size(); i++)
	{
		std::string& n = quest_items[i];
		if (n == item_name)
		{
			quest_items.erase(quest_items.begin()+i);
			return true;
		}
	}
	return false;
}
bool has_quest_item(std::string item_name)
{
	for (unsigned int i = 0; i < quest_items.size(); i++)
	{
		std::string& n = quest_items[i];
		if (n == item_name)
		{
			quest_items.erase(quest_items.begin()+i);
			return true;
		}
	}
	return false;
}

Character* get_selected_character()
{
	std::vector<Character*> party;
	load_party(party);
	return party[player::selected_character];
}

void pick_party_member(std::string text, std::function<void(Character*)> cb)
{
	std::vector<Character*> party;
	player::load_party(party);
	int sz = party.size();
	std::vector<std::string> choices;
	for (Character* ch : party)
	{
		choices.push_back(ch->name);
	}
	choices.push_back("Nevermind");
	game::wait_for_choice(text, choices, [=](int i) {
		if (i == 0)
		{
			cb(NULL);
		}
		else if (i <= sz)
		{
			cb(party[i - 1]);
		}
	});
}

void pick_item_from_inventory(std::string text, Character* ch, std::function<void(Item*,int)> cb)
{
	std::vector<std::string> inventory;
	std::vector<std::string> choices;
	player::load_inventory(ch, inventory);
	for (int j = 0; j < (int)inventory.size(); j++)
	{
		Item* item = get_item(inventory[j]);
		if (item != NULL)
		{
			choices.push_back(item->get_text());
		}
	}
	if (!choices.size())
	{
		choices.push_back("Nevermind (there's nothing in in your inventory)");
	}
	else
	{
		choices.push_back("Nevermind");
	}
	game::wait_for_choice(text, choices, [=](int i){
		if (i == (int)choices.size() || i == 0)
		{
			log("Nevermind.");
			cb(NULL, -1);
		}
		else if (i == 99)
		{
			cb(NULL, 99);
		}
		else
		{
			Item* item = get_item(inventory[i-1]);
			cb(item, i-1);
		}
	});
}

void pick_item_from_storage(std::string text, int filter_usable, std::function<void(Item*,int)> cb)
{
	std::vector<std::string> choices;
	std::map<std::string, int> duplicates;
	std::map<int,int> ind_mapping;
	int ctr = 0;
	for (int j = 0; j < (int)item_storage.size(); j++)
	{
		Item* item = get_item(item_storage[j]);
		log_debug(std::to_string(j) + " " + item->name);
		if (item != NULL)
		{
			std::string item_name = item->get_text();
			if ((filter_usable == 1 && !item->usable) || (filter_usable == 2 && item->is_usable_now()) || !filter_usable)
			{
				if (duplicates.find(item_name) == duplicates.end())
				{
					duplicates[item_name] = 1;
					ctr++;
					log_debug(" :: " + item_name + "=" + std::to_string(ctr) + ":" + std::to_string(j));
					ind_mapping[ctr] = j;
				}
				else
				{
					duplicates[item_name]++;
				}
			}
		}
	}
	for (auto it: duplicates)
	{
		choices.push_back(it.first + (it.second > 1 ? " (" + std::to_string(it.second) + ")" : ""));
	}
	if (!choices.size())
	{
		choices.push_back("Nevermind (no items to pick from backpack)");
	}

	game::wait_for_many_choices(text, choices, [=](int i){
		if (i == (int)choices.size() + 1 || i == 0)
		{
			cb(NULL, -1);
		}
		else if (i == 99)
		{
			cb(NULL, 99);
		}
		else
		{
			auto it = ind_mapping.find(i);
			log_debug("Selected " + std::to_string(i) + " % " + std::to_string(it->second));
			Item* item = get_item(item_storage[it->second]);
			cb(item, it->second);
		}
	});
}

void pick_usable_item_from_storage(std::string text, std::function<void(Item*,int)> cb)
{
	std::vector<std::string> choices;
	std::map<std::string, int> duplicates;
	std::map<int,int> ind_mapping;
	int ctr = 0;
	for (int j = 0; j < (int)item_storage.size(); j++)
	{
		Item* item = get_item(item_storage[j]);
		if (item != NULL && item->is_usable_now())
		{
			std::string item_name = item->get_text();
			if (duplicates.find(item_name) == duplicates.end())
			{
				duplicates[item_name] = 1;
				ctr++;
				ind_mapping[ctr] = j;
			}
			else
			{
				duplicates[item_name]++;
			}
		}
	}

	for (auto it: duplicates)
	{
		choices.push_back(it.first + (it.second > 1 ? " (" + std::to_string(it.second) + ")" : ""));
	}

	if (!choices.size())
	{
		choices.push_back("Nevermind (nothing usable in backpack)");
	}
	game::wait_for_many_choices(text, choices, [=](int i){
		if (i == (int)choices.size() + 1 || i == 0)
		{
			cb(NULL, -1);
		}
		else
		{
			auto it = ind_mapping.find(i);
			Item* item = get_item(item_storage[it->second]);
			cb(item, it->second);
		}
	});
}

void pick_inventory_slot(std::string text, Character* ch, std::function<void(int)> cb)
{
	std::vector<std::string> inventory;
	std::vector<std::string> choices;
	player::load_inventory(ch, inventory);
	for (int j = 0; j < (int)inventory.size(); j++)
	{
		Item* item = get_item(inventory[j]);
		if (item != NULL)
		{
			choices.push_back(item->get_text());
		}
	}

	if((int)choices.size() - 1 < max_inventory_slots)
	{
		choices.push_back("(Last Slot)");
	}
	choices.push_back("Nevermind");

	game::wait_for_choice(text, choices, [=](int i){
		if (i == (int)choices.size() || i == 0)
		{
			cb(-1);
		}
		else
		{
			cb(i-1);
		}
	});
}

//----------------------------------------------------------------------------------------

void log_inventory(Character* ch)
{
	std::vector<std::string> inventory;
	player::load_inventory(ch, inventory);
	log(" " + ch->name + "'s Inventory: ");

	if(!ch->name.size())
	{
		log("  (no items)");
	}

	for (int j = 0; j < max_inventory_slots; j++)
	{
		Item* item = NULL;
		if (j < (int)inventory.size())
		{
			item = get_item(inventory[j]);
		}
		if (item != NULL)
		{
			log( "Slot " + std::to_string(j+1) + ": " + item->get_text());
		}
		else
		{
			log( "Slot " + std::to_string(j+1) + ": ");
		}
	}
}

void log_character(Character* ch)
{
	std::stringstream ss;
	log(" Character: " + ch->name);

	std::vector<std::vector<std::string>> columns;
	std::vector<std::string> col1;
	std::vector<std::string> col2;

	ss << "HP     = " << ch->hp << "/" << ch->max_hp;
	col1.push_back(ss.str());
	ss.str("");

	ss << "AP     = " << ch->ap << "/" << ch->max_ap;
	col1.push_back(ss.str());
	ss.str("");

	ss << "Attack = " << ch->attack;
	col1.push_back(ss.str());
	ss.str("");

	ss << "Magic  = " << ch->magic;
	col1.push_back(ss.str());
	ss.str("");

	ss << "AC     = " << ch->armor_class;
	col1.push_back(ss.str());
	ss.str("");

	ss << "DMG    = " << ch->max_damage;
	col1.push_back(ss.str());
	ss.str("");

	// ss << "MFatigue = " << ch->magic_fatigue;
	// col2.push_back(ss.str());
	// ss.str("");

	ss << "DMGVar = " << ch->dmg_variance_percent << "%";
	col2.push_back(ss.str());
	ss.str("");

	ss << "DR     = " << ch->damage_reduction;
	col2.push_back(ss.str());

	columns.push_back(col1);
	columns.push_back(col2);

	std::vector<std::string> col;
	std::vector<std::string> inventory;
	load_inventory(ch, inventory);
	for (int j = 0; j < max_inventory_slots; j++)
	{
		Item* item = NULL;
		if (j < (int)inventory.size())
		{
			item = get_item(inventory[j]);
		}
		if (item != NULL)
		{
			col.push_back("Slot " + std::to_string(j+1) + ": " + item->get_text());
		}
		else
		{
			col.push_back( "Slot " + std::to_string(j+1) + ": ");
		}
	}
	columns.push_back(col);

	game::log_table_n(columns, false);
}

void log_storage()
{
	std::vector<std::vector<std::string>> columns;
	std::map<std::string, int> duplicates;
	float weight = 0;
	float max_weight = 100;
	int num_rows = 8;

	for (int j = 0; j < (int)item_storage.size(); j++)
	{
		Item* item = get_item(item_storage[j]);
		if (item != NULL)
		{
			weight += item->weight;
			std::string item_name = item->name;
			if (duplicates.find(item_name) == duplicates.end())
			{
				duplicates[item_name] = 1;
			}
			else
			{
				duplicates[item_name]++;
			}
		}
	}

	std::stringstream ss;
	ss << " Backpack: " << std::fixed << std::setprecision(2) << weight << "/" << max_weight;
	log(ss.str());

	int ctr = 0;
	std::vector<std::string> col;
	for (auto it: duplicates)
	{
		col.push_back(it.first + (it.second > 1 ? " (" + std::to_string(it.second) + ")" : ""));
		ctr++;
		if (ctr == num_rows)
		{
			columns.push_back(col);
			col.clear();
			ctr = 0;
		}
	}
	columns.push_back(col);

	game::log_table_n(columns, true);
}

void log_player()
{
	std::vector<Character*> party;
	player::load_party(party);
	log(" Party: ");
	std::vector<std::vector<std::string>> columns;
	std::vector<std::string> col1 = { "", "HP", "AP", "Attack", "AC", "DMG", "DMGVar", "Magic", "DR" };
	columns.push_back(col1);
	for (int i = 0; i < (int)party.size(); i++)
	{
		Character* ch = party[i];
		std::vector<std::string> col;
		col.push_back(ch->name);
		col.push_back(std::to_string(ch->hp) + "/" + std::to_string(ch->max_hp));
		col.push_back(std::to_string(ch->ap) + "/" + std::to_string(ch->max_ap));
		col.push_back(std::to_string(ch->attack));
		col.push_back(std::to_string(ch->armor_class));
		col.push_back(std::to_string(ch->max_damage));
		col.push_back(std::to_string(ch->dmg_variance_percent) + "%");
		col.push_back(std::to_string(ch->magic));
		col.push_back(std::to_string(ch->damage_reduction));
		columns.push_back(col);
	}

	game::log_table_n(columns, false);
}

//----------------------------------------------------------------------------------------

struct InventoryMenu {
	Character* from;
	Character* to;
	int from_index;
	int to_index;
	Item* item;
} inventory_menu;

void _show_drop_menu()
{
	log("");
	game::allow_esc_while_waiting();
	if (game::mode == "explore")
		game::allow_tab_while_waiting();
	pick_item_from_storage("Drop which item?", STORAGE_FILTER_NONE, [=](Item* item, int ind){
		if (ind == -1)
		{
			show_inventory_menu();
			return;
		}
		if (ind == 99) //tab pressed
		{
			player::select_character(-1);
			Character* ch = player::get_selected_character();
			inventory_menu.from = ch;
			_show_drop_menu();
			return;
		}
		std::vector<std::string> choices = { "Yes, drop it.", "No wait..." };
		game::wait_for_choice("Are you sure you want to drop " + item->name + "?  You will lose it forever.", choices, [=](int i){
			if (i == 1)
			{
				player::unstore_item(item->name);
				log(item->name + " is now gone.");
				game::wait_for_enter([](){
					_show_drop_menu();
				});
			}
			else
			{
				_show_drop_menu();
			}
		});
	});
}

void _show_use_menu()
{
	log("");
	game::allow_esc_while_waiting();
	if (game::mode == "explore")
		game::allow_tab_while_waiting();
	pick_item_from_storage("Use which item?", STORAGE_FILTER_USABLE_NOW, [=](Item* item, int ind){
		if (ind == -1)
		{
			show_inventory_menu();
			return;
		}
		else if (ind == 99)
		{
			player::select_character(-1);
			Character* ch = player::get_selected_character();
			inventory_menu.from = ch;
			_show_use_menu();
			return;
		}

		log(inventory_menu.from->name + " uses: " + item->get_text());
		item->on_use(inventory_menu.from);
		unstore_item(item->name);
		if (game::mode == "explore")
		{
			show_inventory_menu();
		}
		else if (game::mode == "combat")
		{
			Character* ch = combat::get_active_character();
			ch->ap = 0;
			combat::after_action();
		}
	});
}

void show_inventory_menu()
{
	Character* ch = player::get_selected_character();
	inventory_menu.from = ch;
	log(" Inventory:");
	std::vector<std::string> choices = {
		"Use",
		"Trade",
		"List",
		"Drop"
	};
	if (game::mode == "explore")
	{
		choices.push_back("From Backpack");
		choices.push_back("To Backpack");
		game::allow_tab_while_waiting();
	}
	game::allow_esc_while_waiting();
	choices.push_back("Nevermind");
	game::wait_for_choice("", choices, [=](int i){
		if (i == 99) //tab pressed
		{
			player::select_character(-1);
			Character* ch = player::get_selected_character();
			inventory_menu.from = ch;
		}
		else if (i == 0) //esc
		{
			return;
		}
		else if (i == 1) // use
		{
			_show_use_menu();
		}
		else if (i == 2) // trade
		{
			game::allow_esc_while_waiting();
			pick_item_from_inventory("Trade which item?", ch, [=](Item* item, int ind){
				if (ind == -1)
				{
					show_inventory_menu();
					return;
				}
				if (ind == 99)
				{
					return;
				}

				inventory_menu.item = item;
				inventory_menu.from_index = ind;
				game::allow_esc_while_waiting();
				pick_party_member("Trade to whom", [=](Character* ch2){
					if (ch2 == NULL)
					{
						show_inventory_menu();
						return;
					}

					inventory_menu.to = ch2;
					if (ch2 == inventory_menu.from)
					{
						log("You cannot transfer to yourself.");
					}
					else
					{
						if ((int)ch2->items.size() >= max_inventory_slots)
						{
							game::allow_esc_while_waiting();
							pick_inventory_slot("Replace which item in inventory of " + ch2->name + "?", ch, [=](int slot_ind){
								inventory_menu.to_index = slot_ind;
								transfer_item(inventory_menu.from, inventory_menu.from_index, inventory_menu.to, inventory_menu.to_index);
								log(inventory_menu.item->name + " transferred from " + inventory_menu.from->name + " to " + inventory_menu.to->name);
							});
						}
						else
						{
							game::allow_esc_while_waiting();
							inventory_menu.to_index = -1;
							transfer_item(inventory_menu.from, inventory_menu.from_index, inventory_menu.to, inventory_menu.to_index);
							log(inventory_menu.item->name + " transferred to " + inventory_menu.to->name);
						}
					}
					show_inventory_menu();
				});
			});
		}
		else if (i == 3) // list
		{
			log_inventory(inventory_menu.from);
			game::wait_for_enter([](){
				show_inventory_menu();
			});
		}
		else if (i == 4) // drop
		{
			_show_drop_menu();
		}
		else if (i == 5 && game::mode == "explore")
		{
			game::allow_esc_while_waiting();
			pick_item_from_storage("Take which item from backpack?", STORAGE_FILTER_USABLE, [=](Item* item, int ind){
				if (ind == -1)
				{
					show_inventory_menu();
					return;
				}
				inventory_menu.item = item;
				inventory_menu.from_index = ind;
				log("Transferred");
				if ((int)ch->items.size() >= max_inventory_slots)
				{
					game::allow_esc_while_waiting();
					pick_inventory_slot("Replace which item in inventory of " + ch->name + "?", ch, [=](int slot_ind){
						player::remove_item_ind(slot_ind, ch);
						player::store_item(inventory_menu.item->name);
						player::unstore_item(inventory_menu.item->name);
						player::add_item(inventory_menu.item->name, ch);
						log(item->name + " transferred from backpack to " + inventory_menu.from->name + ".");
						game::wait_for_enter([](){
							show_inventory_menu();
						});
					});
				}
				else
				{
					player::add_item(inventory_menu.item->name, ch);
					player::unstore_item(inventory_menu.item->name);
				}
				log(item->name + " transferred from backpack to " + inventory_menu.from->name + ".");
				game::wait_for_enter([](){
					show_inventory_menu();
				});
			});
		}
		else if (i == 6 && game::mode == "explore")
		{
			game::allow_esc_while_waiting();
			pick_item_from_inventory("Store which item from " + inventory_menu.from->name + " in backpack?", ch, [=](Item* item, int ind){
				if (ind == -1)
				{
					show_inventory_menu();
					return;
				}
				player::remove_item_ind(ind, inventory_menu.from);
				player::store_item(item->name);
				log(item->name + " is now in backpack.");
				game::wait_for_enter([](){
					show_inventory_menu();
				});
			});
		}
	});
}

void log_player_data()
{
	for (auto it: player_data)
	{
		log(it.first + ": " + it.second);
	}
}

void print_status_bar()
{
	std::vector<Character*> party;
	player::load_party(party);
	std::stringstream ss;
	Character* ch;
	if (game::mode == "combat")
	{
		ch = combat::get_active_character();
		ss << "AP: " << ch->ap  << " | "; 
	}
	else
	{
		ch = player::get_leader();
		ss << "Coins: " << player::get("coins") << ", ";
	}

	for (int i = 0; i < (int)party.size(); i++)
	{
		Character* ch = party[i];
		if (ch == player::get_selected_character())
		{
			ss << get_text(ch->name, "b_cyan", "b_black") << ":";
		}
		else
		{
			ss << get_text(ch->name, "", "") << ":";
		}

		if (ch->is_dead())
		{
			ss << get_text("KO'd", "b_red", "black");
		}
		else
		{
			if (game::mode == "combat")
			{
				bool has_status_effect = false;
				for (unsigned int i = 0; i < combat::status_effects.size(); i++)
				{
					StatusEffect* se = combat::status_effects[i];
					if (se->ch == ch)
					{
						if (!has_status_effect)
							ss << "{";
						has_status_effect = true;
						ss << se->icon;
					}
				}		
				if (has_status_effect)
					ss << "}:";
			}
			ss << get_text(std::to_string(ch->hp), ch->hp == ch->max_hp ? "green" : "yellow", "");
			ss << "/" << ch->max_hp;
		}
		ss << ((i < (int)party.size() - 1) ? ", " : "");
	}
	ss << "              " << std::endl;
	draw_text(ss);
}
}