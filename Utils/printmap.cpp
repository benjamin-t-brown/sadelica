#include <iostream>
#include <map>
#include <iostream>
#include <fstream>

#include "../Game.h"
#include "../World.h"

using namespace world;

int width = 10;
int height = 10;
int map_width = 25;
int map_height = 25;

void print_html_style()
{
	std::string colors[8] = { "black", "red", "green", "yellow", "blue", "pink", "cyan", "white" };
	int num_colors = 8;
	std::cout << "<style>" << std::endl;
	std::cout << "body { background-color: black; color: white; font-family: monospace; font-size: 16px; }" << std::endl;
	std::cout << "span { float: left; width: 9px; cursor:default }" << std::endl;
	std::cout << ".div-row { height: 19px; width: " << ( width * map_width * 9 ) <<  "px}" << std::endl;
	for( int i = 0; i < num_colors; i++ )
	{
		std::string c = colors[ i ];
		if( c == "yellow" )
		{
			c = "orange";
		}
		std::cout << ".tfg" << ( colors[ i ] ) << "{" <<  " color: " << c << " }" << std::endl;
		std::cout << ".tbg" << ( colors[ i ] ) << "{" <<  " background-color: " << c << " }" << std::endl;
	}
	std::cout << "</style>" << std::endl;
	std::cout << "<head><div id='hov' style='position:fixed;background-color:white;color:black;font-size:26'></div>" << std::endl;
	std::cout << "<script>function onhover( ind, ev ){" << std::endl;
	std::cout << "let world_x = Math.floor( ( ind % ( 25 * 10 ) ) / 25 );" << std::endl;
	std::cout << "let world_y = Math.floor( ind / ( 25 * 10 ) / 25 );" << std::endl;
	std::cout << "let map_x =   Math.floor( ( ind % ( 25 * 10 ) ) % 25 );" << std::endl;
	std::cout << "let map_y =   Math.floor( ind / ( 25 * 10 ) % 25  );" << std::endl;
	std::cout << "let obj = document.getElementById( 'hov' );" << std::endl;
	std::cout << "obj.style.left = ev.clientX + 'px'; obj.style.top = (ev.clientY - 50) + 'px';" << std::endl;
	std::cout << "obj.innerHTML = world_x + ' ' + world_y + ' ' + map_x + ' ' + map_y;" << std::endl;
	std::cout << "console.log( world_x, world_y );" << std::endl;
	std::cout << "};</script></head>" << std::endl;
}

void print_tile_html( int id, int ind )
{
	Tile* t = game::get_tile( id );
	std::cout << "\t<span onmouseover=\"onhover(this.id,event)\" id=\"" << ind << "\"" << "class=\"";
	if( t->color.size() )
	{
		std::cout << "tfg" << t->color << " ";
	}
	if( t->bgcolor.size() )
	{
		std::cout << "tbg" << t->bgcolor;
	}
	std::string text = t->text;
	if( text == " " )
	{
		text = "&nbsp";
	}
	std::cout << "\">" << text << "</span>" << std::endl;
}

int main()
{
	std::string a;

	build_world();

	std::cout << "<html>" << std::endl;
	print_html_style();
	std::cout << "<body>" << std::endl;

	int ind = 0;
	for( int y = 0; y < height * map_height; y++ )
	{
		std::cout << "<div class=\"div-row\">" << std::endl;
		for( int x = 0; x < width * map_width; x++ )
		{
			int id = world_map[ y / map_height ][ x / map_width ][ y % map_height ][ x % map_width ];
			//std::cout << ( y / map_height ) << "," << ( x / map_width ) << " ";
			//std::cout << id << " ";
			if( id >= 0 )
			{
				print_tile_html( id, ind );
			}
			else
			{
				print_tile_html( 0, ind );
			}
			ind++;
		}
		std::cout << "</div>" << std::endl;
	}

	std::cout << "</body>" << std::endl;
	std::cout << "</html>" << std::endl;

	return 0;
}
