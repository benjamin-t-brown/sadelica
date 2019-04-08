#include "Items.h"
#include "Game.h"
#include "Player.h"

#include "Content/Content.h"

Item::Item()
{
	usable = false;
	ephemeral = false;
	usable_type = "all";
	on_acquire = ITEM_EMPTY_FUNC;
	on_unacquire = ITEM_EMPTY_FUNC;
	on_use = ITEM_EMPTY_FUNC;
	on_acquire_ephemeral = [](){};
	name = "";
	description = "";
	weight = 1.0;
}

std::string Item::get_text()
{
	return name + " [" + description + "]";
}

void Item::set_usable(std::string type, std::function<void(Character*)> cb)
{
	usable = true;
	usable_type = type;
	if (usable_type != "all" && usable_type != "combat" && usable_type != "explore")
	{
		throw std::runtime_error("Invalid item usable type: " + type);
	}
	on_use = cb;
}
void Item::set_ephemeral(std::function<void()> cb)
{
	ephemeral = true;
	on_acquire_ephemeral = cb;
}

bool Item::is_usable_now()
{
	return usable && (usable_type == "all" || usable_type == game::mode);
}

std::map<std::string, std::shared_ptr<Item>> item_map;
std::map<std::string, std::shared_ptr<Store>> store_map;

void build_stores()
{
	content::build_stores();
}

void build_items()
{
	content::build_items();
}

Store::Store(std::string store_name)
{
	this->name = store_name;
}

void Store::put_item_in_store(std::string name, int cost)
{
	items[name] = cost;
}

void load_store(std::string store_name, std::vector<std::pair<std::string, int>>& store)
{
	if (store_map.find(store_name) != store_map.end())
	{
		auto s = store_map[store_name];
		for (auto it : s->items)
		{
			store.push_back(std::make_pair(it.first, it.second));
		}
	}
}

Item* get_item(std::string item_name)
{
	if (item_map.find(item_name) != item_map.end())
	{
		return item_map[item_name].get();
	}
	else
	{
		return NULL;
	}
}

void add_item(Item* item)
{
	item_map[item->name] = std::shared_ptr<Item>(item);
}

void add_store(Store* store)
{
	store_map[store->name] = std::shared_ptr<Store>(store);
}
