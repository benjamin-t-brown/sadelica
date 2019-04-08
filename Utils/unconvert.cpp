#include <fstream>
#include <iostream>
#include <map>

#include "../World.h"

int main()
{
	build_world();

	std::map<int, int> tiles;

	int default_id = 31;

	tiles[0] = 0;
	tiles[1] = 0;
	tiles[2] = 0;
	tiles[3] = 0;
	tiles[4] = 0;
	tiles[5] = 0;
	tiles[10] = 1;
	tiles[11] = 2;
	tiles[40] = 3;
	tiles[50] = 4;
	tiles[51] = 4;
	tiles[52] = 4;
	tiles[53] = 4;
	tiles[54] = 4;
	tiles[55] = 4;
	tiles[60] = 5;
	tiles[97] = 6;
	tiles[98] = 7;
	tiles[99] = 8;
	tiles[100] = 9;
	tiles[101] = 9;
	tiles[102] = 9;
	tiles[103] = 9;
	tiles[141] = 10;
	tiles[142] = 11;
	tiles[143] = 12;
	tiles[144] = 13;
	tiles[145] = 14;
	tiles[146] = 15;
	tiles[147] = 16;
	tiles[148] = 17;
	tiles[149] = 18;
	tiles[150] = 19;
	tiles[151] = 19;
	tiles[200] = 20;
	tiles[201] = 21;
	tiles[202] = 22;
	tiles[203] = 23;
	tiles[208] = 24;
	tiles[209] = 25;
	tiles[210] = 26;
	tiles[211] = 26;
	tiles[212] = 26;
	tiles[213] = 26;
	tiles[220] = 27;
	tiles[221] = 27;
	tiles[222] = 27;
	tiles[254] = 28;
	tiles[255] = 29;

	std::vector<std::vector<int>> surface;
	for (int i = 0; i < 25 * 10; i++)
	{
		std::vector<int> row;
		for (int j = 0; j < 25 * 10; j++)
		{
			row.push_back(0);
		}
		surface.push_back(row);
	}

	for (int y = 0; y < 10; y++)
	{
		for (int x = 0; x < 10; x++)
		{
			int map[25][25];
			world::load_map(x, y, map);
			for (int i = 0; i < 25; i++)
			{
				for (int j = 0; j < 25; j++)
				{
					int old_map_id = map[i][j];
					int new_id;
					if (tiles.find(old_map_id) != tiles.end())
					{
						new_id = tiles[old_map_id] + 1;
					}
					else
					{
						new_id = default_id;
					}
					surface[y * 25 + i][x * 25 + j] = new_id;
				}
			}
		}
	}

	int ctr = 0;
	for (int i = 0; i < 25 * 10; i++)
	{
		std::vector<std::string> row;
		for (int j = 0; j < 25 * 10; j++)
		{
			std::cout << surface[i][j] << ",";
			if (ctr && ctr % 500 == 0)
				std::cout << "\n";
			ctr++;
		}
	}

	return 0;
}
