#pragma once

#include "Game.h"

namespace ai
{
void do_behavior(Character* ch);
void get_offset_towards(int& x, int& y, int x1, int y1, int x2, int y2);
void get_offset_towards(int& x, int& y, Character* ch_from, Character* ch_to);
float get_distance_to(Character* ch_from, Character* ch_to);
Character* get_nearest_enemy(Character* ch);
Character* get_random_player();
Character* get_random_cpu();
Character* get_random_injured_cpu();
std::pair<int,int> get_path_next(int x1, int y1, int x2, int y2, int map[GAME_YMAPSIZE][GAME_XMAPSIZE]);
}