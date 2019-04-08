#pragma once

#include "Game.h"

class Encounter;

void build_world();

namespace world
{
//extern std::vector<Encounter*> encounters[GAME_WORLDSIZE][GAME_WORLDSIZE];
extern std::map<std::string, std::vector<Encounter*>> stored_encounters;
extern std::map<std::string, std::string> trigger_locations;
extern int world_map[GAME_WORLDSIZE][GAME_WORLDSIZE][GAME_YMAPSIZE][GAME_XMAPSIZE];
extern int fog_of_war[GAME_WORLDSIZE][GAME_WORLDSIZE][GAME_YMAPSIZE][GAME_XMAPSIZE];
extern std::map<std::string,int> changed_tiles;

bool blocks_sight(int x, int y, int map[GAME_YMAPSIZE][GAME_XMAPSIZE]);
bool is_wall(int tile_id);
bool is_door(int tile_id);
bool is_open_door(int tile_id);
void set_visibility(int chx, int chy, int map[GAME_YMAPSIZE][GAME_XMAPSIZE], int vismap[GAME_YMAPSIZE][GAME_XMAPSIZE]);
void assign_fog_of_war(int world_x, int world_y, int vismap[GAME_YMAPSIZE][GAME_XMAPSIZE]);
void load_map(int x, int y, int map[GAME_YMAPSIZE][GAME_XMAPSIZE]);
void set_tile_id(int v, int x, int y, int i, int j);
void set_fog_of_war(int v, int x, int y, int i, int j);
bool has_explored(int x, int y, int i, int j);
void assign(int x, int y, int b[GAME_YMAPSIZE][GAME_XMAPSIZE], int a[GAME_WORLDSIZE][GAME_WORLDSIZE][GAME_YMAPSIZE][GAME_XMAPSIZE]);
}
