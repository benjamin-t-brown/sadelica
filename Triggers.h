#pragma once

#include <functional>
#include <map>
#include <vector>
#include <string>
#include <memory>

class Character;

class Sign
{
  public:
	std::string text;
	Sign(std::string text, int x, int y, int i, int j);
};

class Chest
{
	int x;
	int y;
	int i;
	int j;
  public:
  	std::vector<std::string> item_names;
  	Chest(std::string names, int x, int y, int i, int j);
  	std::string save();
};

#define TRIGGER_LIST_LENGTH 1000

void build_signs();
void build_triggers();
void apply_trigger(std::string id, Character* ch);
std::string get_sign_text(int x, int y, int i, int j);
bool show_chest(int x, int y, int i, int j);
bool call_trigger(int x, int y, int i, int j, Character* ch);

namespace triggers
{
//extern std::function<void(Character*)> trigger_list[TRIGGER_LIST_LENGTH];
extern std::map<std::string, std::function<void(Character*)>> trigger_list;
extern std::map<std::string, std::shared_ptr<Sign>> signs;
extern std::map<std::string, std::shared_ptr<Chest>> chests;

bool once(std::string key);
bool once_reset_tile(std::string key, int tile_id=1);
}
