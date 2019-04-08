#include <iostream>
#include <map>
#include <iostream>
#include <fstream>

int main()
{
	std::string a;

	int width = 25;
	int height = 25;

	std::map< int, int > tiles;

	tiles[ 0 ] = 0; //grass (steppable)
	tiles[ 1 ] = 10; //dirt
	tiles[ 5 ] = 40; //floor
	tiles[ 2 ] = 150; //water (unsteppable)
	tiles[ 3 ] = 100; //tree
	tiles[ 4 ] = 200; //wall
	tiles[ 6 ] = 50; //cave floor
	tiles[ 7 ] = 220; //cave wall
	tiles[ 8 ] = 210; //cave tree
	tiles[ 9 ] = 208; //fence NS
	tiles[ 10 ] = 209; //fence EW

	char* arr;
	long x;
	std::string map_name = "BanshireMines.map";
	std::string url = "../AdventureMaps/" + map_name;

	std::cout << "READ: " << url << std::endl;

	std::ifstream ifs( url, std::ios::binary );
	ifs.seekg( 0, std::ios::end );
	x = ifs.tellg();
	ifs.seekg( 0, std::ios::beg );

	arr = new char[ x ];
	ifs.read( arr, x );
	ifs.close();

	std::cout << "int map[GAME_YMAPSIZE][GAME_XMAPSIZE] = {" << std::endl;
	for ( int i = 0; i < width; i++ )
	{
		std::cout << "{";
		for ( int j = 0; j < height; j++ )
		{
			int id = tiles[ arr[ i * width + j ] ];
			std::cout << ( id < 10 ? "    " : id < 100 ? "   " : "  " ) << id << ( j < height - 1 ? "," : "" );
		}
		std::cout << "}" << ( i < width - 1 ? "," : "" ) << std::endl;
	}
	std::cout << "};";

	delete [] arr;
}
