#include "World.h"
#include "Game.h"
#include <cmath>
#include <cstring>

#include "Content/Content.h"

namespace world
{
std::map<std::string, std::vector<Encounter*>> stored_encounters;
std::map<std::string, std::string> trigger_locations;
std::map<std::string,int> changed_tiles;

int world_map[GAME_WORLDSIZE][GAME_WORLDSIZE][GAME_YMAPSIZE][GAME_XMAPSIZE];
int fog_of_war[GAME_WORLDSIZE][GAME_WORLDSIZE][GAME_YMAPSIZE][GAME_XMAPSIZE];

bool blocks_sight(int x, int y, int map[GAME_YMAPSIZE][GAME_XMAPSIZE])
{
	if (y < 0 || x < 0 || y >= GAME_YMAPSIZE || x >= GAME_XMAPSIZE)
	{
		return true;
	}

	static std::vector<int> view_throughs = {
		141, 142, 143, 144, 145, 146, 147, 148, 149,
		150, 151, 208, 209, 201, 204, 230, 232, 233,
		237, 238, 239
	};

	int n = map[y][x];
	return n >= GAME_MOVE_THRESHOLD
		&& std::find(view_throughs.begin(), view_throughs.end(), n) == view_throughs.end();
}

bool is_wall(int tile_id)
{
	return tile_id > GAME_MOVE_THRESHOLD;
}

bool is_door(int tile_id)
{
	static std::vector<int> doors = { 240, 235, 99 };
	return std::find(doors.begin(), doors.end(), tile_id) != doors.end();
}

bool is_open_door(int tile_id)
{
	return is_door(tile_id + 1);
}

void set_visibility(int chx, int chy, int map[GAME_YMAPSIZE][GAME_XMAPSIZE], int vismap[GAME_YMAPSIZE][GAME_XMAPSIZE])
{
	std::memset(vismap, 0, sizeof(vismap[0][0]) * GAME_YMAPSIZE * GAME_XMAPSIZE);
	auto get_adjacent = [=](float x, float y) {
		float vx = chx - x;
		float vy = chy - y;
		int dx, dy;
		if (vx == 0)
		{
			dx = 0;
		}
		else
		{
			dx = vx / abs(vx);
		}

		if (vy == 0)
		{
			dy = 0;
		}
		else
		{
			dy = vy / abs(vy);
		}
		return std::make_pair(x + dx, y + dy);
	};

	auto cast_ray = [=](float x1, float y1, float x2, float y2) {
		x1 += 0.5;
		x2 += 0.5;

		float vx = x2 - x1;
		float vy = y2 - y1;

		int visibility = 1;
		int iter = 25;

		int prev_check_x = floor(x1);
		int prev_check_y = floor(y1);

		for (int i = 0; i <= iter; i++)
		{
			int check_x = floor(x1 + (i * vx) / iter);
			int check_y = floor(y1 + (i * vy) / iter);

			if (check_x == prev_check_x && check_y == prev_check_y)
			{
				continue;
			}

			vismap[check_y][check_x] = visibility;

			std::pair<int, int> adj = get_adjacent(check_x, check_y);
			std::pair<int, int> prev_adj = get_adjacent(prev_check_x, prev_check_y);
			bool hack = vismap[prev_check_y][prev_check_x] == 1 && !blocks_sight(adj.first, adj.second, map) && !blocks_sight(prev_adj.first, prev_adj.second, map);

			if (hack)
			{
				vismap[check_y][check_x] = 1;
			}

			if (blocks_sight(check_x, check_y, map))
			{
				visibility = 2;
			}

			if (check_x != prev_check_x)
			{
				prev_check_x = check_x;
			}
			if (check_y != prev_check_y)
			{
				prev_check_y = check_y;
			}
		}
	};

	for (int i = 0; i < GAME_XMAPSIZE; i++)
	{
		float x2 = i;
		float y2_top = 0;
		float y2_bottom = GAME_YMAPSIZE - 1;

		//printf( " cast %d, %d to %d, %d\n", chx, chy, x2, y2_top );
		//printf( " cast %d, %d to %d, %d\n", chx, chy, x2, y2_bottom );

		cast_ray(chx, chy, x2, y2_top);
		cast_ray(chx, chy, x2, y2_bottom);
	}
	for (int i = GAME_YMAPSIZE - 1; i >= 0; i--)
	{
		float x2_left = 0;
		float x2_right = GAME_XMAPSIZE - 1;
		float y2 = i;

		//printf( " cast %d, %d to %d, %d\n", chx, chy, x2_left, y2 );
		//printf( " cast %d, %d to %d, %d\n", chx, chy, x2_right, y2 );

		cast_ray(chx, chy, x2_left, y2);
		cast_ray(chx, chy, x2_right, y2);
	}

	vismap[chy][chx] = 4;
}

void assign_fog_of_war(int world_map_x, int world_map_y, int vismap[GAME_YMAPSIZE][GAME_XMAPSIZE])
{
	for (int i = 0; i < GAME_YMAPSIZE; i++)
	{
		for (int j = 0; j < GAME_XMAPSIZE; j++)
		{
			int visible = vismap[i][j] == 1 ? 1 : 0;
			if (visible)
			{
				set_fog_of_war(1, world_map_x, world_map_y, j, i);
			}
		}
	}
}

void set_tile_id(int v, int x, int y, int i, int j)
{
	std::stringstream ss;
	ss << x << "," << y << "," << j << "," << i;
	changed_tiles[ss.str()] = v;
	if (x == game::current_world_x && y == game::current_world_y)
	{
		game::map[j][i] = v;
	}
	world_map[y][x][j][i] = v;
}

void set_fog_of_war(int v, int x, int y, int i, int j)
{
	fog_of_war[y][x][j][i] = v;
}

bool has_explored(int x, int y, int i, int j)
{
	return fog_of_war[y][x][j][i];
}

int vary_grass()
{
	int id = 0;
	return game::get_random(id, id + 6);
}

int vary_dirt()
{
	int id = 11;
	return game::get_random(id, id + 6);
}

int vary_tree()
{
	int id = 100;
	return game::get_random(id, id + 4);
}

int vary_water()
{
	int id = 150;
	return game::get_random(id, id + 2);
}

int vary_cave_floor()
{
	int id = 50;
	return game::get_random(id, id + 6);
}

int vary_cave_tree()
{
	int id = 210;
	return game::get_random(id, id + 4);
}

int vary_cave_wall()
{
	int id = 220;
	return game::get_random(id, id + 3);
}

// Used to assign a 2d array "b" to the position x, y inside a 4d array "a"
void assign(int x, int y, int b[GAME_YMAPSIZE][GAME_XMAPSIZE], int a[GAME_WORLDSIZE][GAME_WORLDSIZE][GAME_YMAPSIZE][GAME_XMAPSIZE])
{
	for (int i = 0; i < GAME_YMAPSIZE; i++)
	{
		for (int j = 0; j < GAME_XMAPSIZE; j++)
		{

			int id = b[i][j];
			if (id == 0 || id == 1)
			{
				id = vary_grass();
			}
			else if (id == 11)
			{
				id = vary_dirt();
			}
			else if (id == 100)
			{
				id = vary_tree();
			}
			else if (id == 150)
			{
				id = vary_water();
			}
			else if (id == 50)
			{
				id = vary_cave_floor();
			}
			else if (id == 210)
			{
				id = vary_cave_tree();
			}
			else if (id == 220)
			{
				id = vary_cave_wall();
			}
			a[y][x][i][j] = id;
		}
	}
}

void load_map(int x, int y, int map[GAME_YMAPSIZE][GAME_XMAPSIZE])
{
	for (int i = 0; i < GAME_YMAPSIZE; i++)
	{
		for (int j = 0; j < GAME_XMAPSIZE; j++)
		{
			map[i][j] = world_map[y][x][i][j];
		}
	}
}
}

using namespace world;

void build_world()
{
	std::memset(world_map, 0, sizeof(world_map[0][0][0][0]) * GAME_YMAPSIZE * GAME_XMAPSIZE * GAME_WORLDSIZE * GAME_WORLDSIZE);
	std::memset(fog_of_war, 0, sizeof(world_map[0][0][0][0]) * GAME_YMAPSIZE * GAME_XMAPSIZE * GAME_WORLDSIZE * GAME_WORLDSIZE);

	trigger_locations.clear();
	stored_encounters.clear();
	changed_tiles.clear();

	content::build_world();
}
