#include "Game.h"
#include "Characters.h"
#include "Combat.h"
#include "DrawText.h"
#include "Encounters.h"
#include "Explore.h"
#include "Player.h"
#include "Triggers.h"
#include "World.h"
#include "Items.h"
#include "Magic.h"

#include <cstdlib>
#include <fstream>
#include <algorithm>

Tile::Tile(std::string text, std::string color, std::string bgcolor, std::string description)
{
	this->text = text;
	this->color = color;
	this->bgcolor = bgcolor;
	this->description = description;
}

void Tile::print(bool use_colors)
{
	if (use_colors)
	{
		draw_text(get_text(text, color, bgcolor));
	}
	else
	{
		draw_text(get_text(text, "white", "black"));
	}
}

FloatingText::FloatingText(std::string text, int x, int y)
{
	this->x = x;
	this->y = y;
	this->text = text;
}

char FloatingText::get_drawable_character(int _x, int _y)
{
	if (y != _y)
	{
		return 27;
	}

	int i = _x - x;
	if (i < (int)text.size() && i >= 0)
	{
		return text[i];
	}
	else
	{
		return 27;
	}
}

namespace game
{
int current_world_x = 0;
int current_world_y = 0;
int map[GAME_YMAPSIZE][GAME_XMAPSIZE];
int vismap[GAME_YMAPSIZE][GAME_XMAPSIZE];
int camera_x = 0;
int camera_y = 0;
int camera_width = GAME_CAMERA_WIDTH;
int camera_height = GAME_CAMERA_HEIGHT;
bool running = true;
std::string encounter_name;

Character* active_characters[GAME_MAX_CHARACTERS];

std::function<void()> wait_cb;
std::function<void(int)> wait_choice_cb;
std::function<void(std::string&)> wait_arrow_cb;
std::function<void(int, int)> wait_target_cb;
std::function<void()> on_encounter_end_cb = GAME_EMPTY_FUNC;
int num_wait_choices = 0;
bool wait_for_continue = false;
bool wait_for_choice_continue = false;
bool wait_for_arrow_continue = false;
bool wait_for_target_continue = false;
int wait_target_size = 1;
bool wait_allow_tab = false;
bool wait_allow_esc = false;
std::vector<std::pair<int,int>> wait_select_coords;
int wait_select_x = 0;
int wait_select_y = 0;
std::vector<FloatingText*> floating_text;
std::string mode = "explore";
bool autodraw_enabled = false;
bool input_is_enabled = true;
int wait_frame = 0;

std::vector<std::pair<int, std::function<void()>>> action_queue_seq;

std::map<int, Tile*> tiles = {
	// floor-based tiles
	{0, new Tile("_", "green", "", "Grass")}, // grass
	//{1, new Tile(" ", "green", "", "Grass")},
	{1, new Tile(" ", "green", "", "Grass")},
	{2, new Tile(",", "green", "", "Grass")},
	{3, new Tile(".", "green", "", "Grass")},
	{4, new Tile("`", "green", "", "Grass")},
	{5, new Tile("\"", "green", "", "Grass")},
	{10, new Tile("/", "yellow", "", "Dirt path")},     // dirt
	{11, new Tile("-", "yellow", " ", "Dirt")},         // dirt2
	{12, new Tile(" ", "yellow", "", "Dirt")},
	{13, new Tile(",", "yellow", "", "Dirt")},
	{14, new Tile(".", "yellow", "", "Dirt")},
	{15, new Tile("`", "yellow", "", "Dirt")},
	{16, new Tile(":", "yellow", "", "Dirt")},
	{40, new Tile("_", "yellow", "black", "Flooring")}, // indoor-flooring
	{50, new Tile("_", "blue", "", "Cave floor")},      // cave floor
	{51, new Tile(" ", "blue", "", "Cave floor")},
	{52, new Tile(",", "blue", "", "Cave floor")},
	{53, new Tile(".", "blue", "", "Cave floor")},
	{54, new Tile("`", "blue", "", "Cave floor")},
	{55, new Tile("\"", "blue", "", "Cave floor")},
	{60, new Tile("$", "pink", "cyan", "Empty Chest")},
	{61, new Tile("!", "yellow", "", "Something")},
	{62, new Tile("=", "yellow", "pink", "Stairs")},
	{63, new Tile(">", "yellow", "pink", "Stairs Up")},
	{64, new Tile("<", "yellow", "pink", "Stairs Down")},

	{97, new Tile("#", "red", "", "An open gate")},       // gate open
	{98, new Tile("D", "red", "black", "An open door")},  // door open
	{99, new Tile("D", "white", "red", "A closed door")}, // door closed

	// wall-based tiles
	{100, new Tile("T", "black", "green", "A tree")}, // tree
	{101, new Tile("t", "black", "green", "A tree")},
	{102, new Tile("4", "black", "green", "A tree")},
	{103, new Tile("Y", "black", "green", "A tree")},
	{141, new Tile("O", "cyan", "black", "A sack")}, //indoor stuff
	{142, new Tile("H", "cyan", "black", "Shelves")},
	{143, new Tile("%", "cyan", "black", "A plant")},
	{144, new Tile("#", "cyan", "black", "Shelves")},
	{145, new Tile("+", "cyan", "black", "A wooden table")},
	{146, new Tile("[", "cyan", "black", "A bed frame")},
	{147, new Tile("]", "cyan", "black", "A bed frame")},
	{148, new Tile("(", "cyan", "black", "A round table")},
	{149, new Tile(")", "cyan", "black", "A round table")},
	{150, new Tile("~", "white", "blue", "Water")}, // water
	{151, new Tile(" ", "white", "blue", "Water")},
	{200, new Tile("@", "black", "red", "A wall")},            // building wall
	{201, new Tile("#", "white", "red", "A closed gate")},     // gate closed
	{202, new Tile("#", "pink", "yellow", "A bookshelf")},     // bookshelf
	{203, new Tile("&", "yellow", "red", "A sign")},
	{204, new Tile("^", "white", "red", "Castling")}, 
	{208, new Tile("|", "b_white", "b_black", "Fencing")},         // fence NW
	{209, new Tile("=", "b_white", "b_black", "Fencing")},         // fence EW
	{210, new Tile("%", "black", "yellow", "Rocks")}, // cave tree
	{211, new Tile("(", "black", "yellow", "Rocks")},
	{212, new Tile(")", "black", "yellow", "Rocks")},
	{213, new Tile("&", "black", "yellow", "Rocks")},
	{220, new Tile("X", "white", "yellow", "Mountain")}, // cave wall
	{221, new Tile("x", "white", "yellow", "Mountain")},
	{222, new Tile(".", "white", "yellow", "Mountain")},
	{230, new Tile("O", "white", "red", "A window")},
	{231, new Tile("@", "black", "b_yellow", "A wall")},            // building wall
	{232, new Tile("#", "black", "yellow", "A closed gate")},
	{233, new Tile("O", "white", "yellow", "A window")},
	{234, new Tile("D", "yellow", "black", "An open door")},  // door open
	{235, new Tile("D", "white", "yellow", "A closed door")}, // door closed
	{236, new Tile("@", "black", "blue", "An intricate wall")},  // building wall
	{237, new Tile("#", "white", "blue", "A closed gate")},
	{238, new Tile("o", "white", "blue", "A window")},
	{239, new Tile("D", "blue", "black", "An open door")},
	{240, new Tile("D", "white", "blue", "A closed door")},

	{253, new Tile(" ", "", "", "Nothing")}, 
	{254, new Tile("X", "pink", "cyan", "X")}, // special
	{255, new Tile("$", "pink", "cyan", "Empty Chest")},

	// encounter starting points
	{-1000, new Tile(" ", "green", "", "")},
	{-1001, new Tile(" ", "green", "", "")},
	{-2000, new Tile(" ", "white", "white", "")}, //fog of war
};

void init()
{
	current_world_x = 0;
	current_world_y = 0;
	camera_x = 0;
	camera_y = 0;
	camera_width = GAME_CAMERA_WIDTH;
	camera_height = GAME_CAMERA_HEIGHT;
	running = true;
	on_encounter_end_cb = GAME_EMPTY_FUNC;
	num_wait_choices = 0;
	wait_for_continue = false;
	wait_for_choice_continue = false;
	wait_for_arrow_continue = false;
	wait_for_target_continue = false;
	wait_target_size = 1;
	wait_allow_tab = false;
	wait_allow_esc = false;
	wait_select_x = 0;
	wait_select_y = 0;
	mode = "explore";
	autodraw_enabled = false;
	input_is_enabled = true;
	wait_frame = 0;

	build_world();
	build_signs();
	build_triggers();
	build_items();
	build_characters();
	build_encounters();
	build_stores();
	build_magic();
	build_player();
}

void add_tile(int id, std::string text, std::string color, std::string bgcolor)
{
	tiles[id] = new Tile(text, color, bgcolor, "");
}
Tile* get_tile(int id)
{
	return tiles[id];
}

void disable_input()
{
	game::input_is_enabled = false;
}
void enable_input()
{
	game::input_is_enabled = true;
}

std::string offset_to_direction(int x_offset, int y_offset)
{
	if (y_offset == 0)
	{
		return x_offset > 0 ? "e" : "w";
	}
	else if (x_offset == 0)
	{
		return y_offset > 0 ? "s" : "n";
	}
	else if (y_offset < 0)
	{
		return x_offset > 0 ? "ne" : "nw";
	}
	else
	{
		return x_offset > 0 ? "se" : "sw";
	}
}

void direction_to_offset(std::string d, int arr[2])
{
	if (d == "n")
	{
		arr[0] = 0;
		arr[1] = -1;
	}
	else if (d == "s")
	{
		arr[0] = 0;
		arr[1] = 1;
	}
	else if (d == "e")
	{
		arr[0] = 1;
		arr[1] = 0;
	}
	else if (d == "w")
	{
		arr[0] = -1;
		arr[1] = 0;
	}
	else if (d == "ne")
	{
		arr[0] = 1;
		arr[1] = -1;
	}
	else if (d == "se")
	{
		arr[0] = 1;
		arr[1] = 1;
	}
	else if (d == "nw")
	{
		arr[0] = -1;
		arr[1] = -1;
	}
	else if (d == "sw")
	{
		arr[0] = -1;
		arr[1] = 1;
	}
}

void direction_to_offset(std::string d, int& x_offset, int& y_offset)
{
	int offsets[2];
	direction_to_offset(d, offsets);
	x_offset = offsets[0];
	y_offset = offsets[1];
}

// includes lower bound, does not include uppper bound
int get_random(int low, int high)
{
	if (low == high)
		return low;
	return (rand() % (high - low)) + low;
}

// return true/false given a percent chance of something occurring
bool get_probability(int percent)
{
	int i = game::get_random(0, 100) + 1;
	return i > 100 - percent;
}

std::string pick_string(std::vector<std::string> arr)
{
	if (!arr.size())
		return "";

	return arr[get_random(0, arr.size())];
}

void limit_bounds(int* x, int* y, int maxw, int maxh)
{
	if (*x < 0)
	{
		*x = 0;
	}
	if (*y < 0)
	{
		*y = 0;
	}
	if (*x >= maxw)
	{
		*x = maxw - 1;
	}
	if (*y >= maxh)
	{
		*y = maxh - 1;
	}
}

bool is_out_of_bounds(int x, int y)
{
	if (x < 0 || x >= GAME_XMAPSIZE || y < 0 || y >= GAME_YMAPSIZE)
		return true;
	else
		return false;
}

std::string get_heading(Character* ch)
{
	int vy = ch->y - ch->old_y;
	int vx = ch->x - ch->old_x;
	if (vy < 0 && vx == 0)
	{
		return "n";
	}
	else if (vy > 0 && vx == 0)
	{
		return "s";
	}
	else if (vx < 0 && vy == 0)
	{
		return "w";
	}
	else if (vx > 0 && vy == 0)
	{
		return "e";
	}
	else if (vy < 0 && vx < 0)
	{
		return "n";
	}
	else if (vy < 0 && vx > 0)
	{
		return "n";
	}
	else if (vy > 0 && vx < 0)
	{
		return "s";
	}
	else if (vx > 0 && vx > 0)
	{
		return "s";
	}
	else
	{
		return "n";
	}
}

std::string get_cardinal(Character* ch)
{
	if (ch->y <= 1)
	{
		return "n";
	}
	else if (ch->y >= 23)
	{
		return "s";
	}
	else if (ch->x <= 1)
	{
		return "w";
	}
	else if (ch->x >= 23)
	{
		return "e";
	}
	else
	{
		return "n";
	}
}

std::vector<int> to4d(int x, int y)
{
	std::vector<int> ret;
	ret.push_back(x / GAME_XMAPSIZE);
	ret.push_back(y / GAME_YMAPSIZE);
	ret.push_back(x % GAME_XMAPSIZE);
	ret.push_back(y % GAME_YMAPSIZE);
	return ret;
}

void clear_characters()
{
	for (int i = 0; i < GAME_MAX_CHARACTERS; i++)
	{
		game::active_characters[i] = NULL;
	}
}

void change_map(int world_x, int world_y)
{
	game::current_world_x = world_x;
	game::current_world_y = world_y;
	world::load_map(world_x, world_y, game::map);
	load_characters(world_x, world_y, game::active_characters);
}

void change_map(Character* ch)
{
	std::stringstream ss;
	change_map(ch->world_x, ch->world_y);
}

void add_character_to_current_map(Character* ch, int x, int y)
{
	for (int i = 0; i < GAME_MAX_CHARACTERS; i++)
	{
		if (game::active_characters[i] == NULL)
		{
			ch->x = x;
			ch->y = y;
			ch->world_x = game::current_world_x;
			ch->world_y = game::current_world_y;
			game::active_characters[i] = ch;
		}
	}
}

void set_mode(std::string m)
{
	if (m == "combat" && mode != "combat")
	{
		combat::begin_combat(game::encounter_name);
	}
	else if (m == "explore" && mode == "combat")
	{
		combat::end_combat();
	}
	mode = m;
}

void start_encounter(
	std::string encounter_name,
	std::function<void()> on_start,
	std::function<void()> on_end)
{
	game::encounter_name = "heal_cave_boss";
	game::set_mode("combat");
	on_start();
	on_encounter_end_cb = on_end;
}

bool is_waiting()
{
	return wait_for_continue || wait_for_choice_continue || wait_for_arrow_continue || wait_for_target_continue;
}

void wait_for_enter(std::function<void()> cb)
{
	if (game::wait_for_choice_continue)
	{
		return;
	}
	wait_cb = cb;
	game::wait_for_continue = true;
	log_special(" Press 'Enter' to continue...");
}
void wait_for_choice(std::string text, std::vector<std::string>& choice_text, std::function<void(int)> cb)
{
	num_wait_choices = choice_text.size();
	wait_choice_cb = cb;
	if (text.size())
		log_dialog(text);
	for (int i = 0; i < (int)choice_text.size(); i++)
	{
		log(std::to_string(i + 1) + ". " + choice_text[i]);
	}
	game::wait_for_choice_continue = true;
	log_special(" Press number to indicate choice...");
}
void wait_for_many_choices(
	std::string text,
	std::vector<std::string>& choice_text,
	std::function<void(int)> cb,
	unsigned int many_choices_start_ind)
{
	if (choice_text.size() > 7)
	{
		int choices_per_page = 5;

		unsigned int many_choices_end_ind = many_choices_start_ind + choices_per_page;
		if (many_choices_end_ind > choice_text.size())
			many_choices_end_ind = choice_text.size();
		std::vector<std::string> choices = std::vector<std::string>(
			choice_text.begin() + many_choices_start_ind,
			choice_text.begin() + many_choices_end_ind);

		choices.insert(choices.begin(), " {forward 1 page}");
		choices.insert(choices.begin(), " {back 1 page}");
		choices.push_back("Nevermind");
		wait_for_choice(text, choices, [=](int i){
			auto cb2 = cb;
			if (i == 99)
			{
				cb(99);
			}
			else if (i == 1) // back
			{
				auto cp = choice_text;
				int new_start = many_choices_start_ind - choices_per_page;
				if (new_start < 0)
					new_start = many_choices_start_ind;

				wait_for_many_choices(text, cp, cb, new_start);
			}
			else if (i == 2) // forward
			{
				auto cp = choice_text;
				int new_start = many_choices_start_ind + choices_per_page;
				if (new_start >= (int)choice_text.size())
					new_start = many_choices_start_ind;

				wait_for_many_choices(text, cp, cb, new_start);
			}
			else if (i == (int)choices.size() || i == 0) // nevermind
			{
				cb(0);
			}
			else
			{
				int selected_ind = i - 2 + many_choices_start_ind;
				cb(selected_ind);
			}
		});
	}
	else
	{
		std::vector<std::string> choices = choice_text;
		choices.push_back("Nevermind");
		wait_for_choice(text, choices, cb);
	}
}
void wait_for_arrow(std::string text, std::function<void(std::string&)> cb)
{
	wait_arrow_cb = cb;
	log(text);
	game::wait_for_arrow_continue = true;
	log_special(" Press arrow to indicate direction...");
}
void _populate_targetting_coords()
{
	wait_select_coords.clear();
	if (wait_target_size == 2)
	{
		for (int i = -1; i <= 1; i++)
		{
			for (int j = -1; j <= 1; j++)
			{
				wait_select_coords.push_back(std::make_pair(wait_select_x + j, wait_select_y + i));
			}
		}
	}
	else
	{
		wait_select_coords.push_back(std::make_pair(wait_select_x, wait_select_y));
	}
}
void wait_for_target(std::function<void(int, int)> cb, int target_size)
{
	wait_target_size = target_size;
	wait_target_cb = cb;
	game::wait_for_target_continue = true;
	wait_select_x = game::camera_x + game::camera_width / 2;
	wait_select_y = game::camera_y + game::camera_height / 2;
	_populate_targetting_coords();
	log_special(" Press arrows to select target, press 'Enter' to continue...");
}
void allow_tab_while_waiting()
{
	game::wait_allow_tab = true;
}
void allow_esc_while_waiting()
{
	game::wait_allow_esc = true;
}

void add_action_seq(std::function<void()> cb, int frames)
{
	game::autodraw_enabled = true;
	action_queue_seq.push_back(std::make_pair(frames, [=]() {
		cb();
		if (action_queue_seq.size() == 1)
		{
			game::autodraw_enabled = false;
		}
	}));
}

bool call_action_seq()
{
	if (action_queue_seq.size())
	{
		auto p = action_queue_seq[0];
		int max_wait_frame = p.first;
		if (wait_frame >= max_wait_frame)
		{
			action_queue_seq[0].second();
			action_queue_seq.erase(action_queue_seq.begin());
			wait_frame = 0;
			return true;
		}
		else
		{
			wait_frame++;
		}
	}

	return false;
}

void add_floating_text(std::string text, int frames, int x, int y)
{
	FloatingText* t = new FloatingText(text, x, y);
	game::add_action_seq([=]() {
		floating_text.push_back(t);
	}, 0);
	game::add_action_seq([=]() {
		for (int i = 0; i < (int)floating_text.size(); i++)
		{
			if (floating_text[i] == t)
			{
				floating_text.erase(floating_text.begin() + i);
				break;
			}
		}
		delete t;
	}, frames);
}

void log_table_n(std::vector<std::vector<std::string>>& columns, bool is_numbered)
{
	if (!columns.size())
	{
		log("INVALID TABLE, CANNOT DRAW");
		return;
	}

	unsigned int height = columns[0].size();

	std::map<int, unsigned int> widths;
	int i = 0;
	for (auto& column: columns)
	{
		unsigned int max_width = 0;
		for (auto& str: column)
		{
			max_width = max_width > str.size() ? max_width : str.size();
		}
		widths[i++] = max_width;
	}

	for (unsigned int row = 0; row < height; row++ )
	{
		int j = 0;
		std::stringstream ss;
		for (auto& it: widths)
		{
			unsigned int width = it.second;
			unsigned int column = it.first;
			if (is_numbered)
			{
				int n = row + 1 + j * height;
				ss << n << ". ";
				if (n == 9)
					ss << " ";
			}
			std::string c1 = "";
			if (columns[column].size() > row)
				c1 = columns[column][row];
			while (c1.size() < width)
			{
				c1 += " ";
			}
			ss << c1 << "  ";
			j++;
		}
		log(ss.str());
	}
}

void print_tile(int tile, bool use_colors)
{
	if (tiles.find(tile) != tiles.end())
	{
		tiles[tile]->print(use_colors);
	}
	else
	{
		std::stringstream ss;
		ss << "No tile with id: " << tile << " exists!";
		log_debug(ss.str());
		throw std::runtime_error(ss.str().c_str());
	}
}

bool print_character(int x, int y)
{
	Character* player = player::get_leader();
	for (int i = 0; i < GAME_MAX_CHARACTERS; i++)
	{
		Character* c = game::active_characters[i];
		if (c != NULL && c->x == x && c->y == y)
		{
			if (c == player)
			{
				c->print();
				return true;
			}
			if (c->is_on_current_map() && !c->is_dead())
			{
				c->print();
				return true;
			}
		}
	}
	return false;
}

void print_map(bool use_visibility)
{
	for (int x = game::camera_x; x < game::camera_width + game::camera_x + 2; x++)
	{
		draw_text("-");
	}

	draw_text(" \n");

	for (int y = game::camera_y; y < game::camera_height + game::camera_y; y++)
	{
		draw_text("|");
		for (int x = game::camera_x; x < game::camera_width + game::camera_x; x++)
		{
			bool is_tile_visible = true;
			if (use_visibility)
			{
				Character* leader = player::get_leader();
				if (game::vismap[y][x] == 2 || game::vismap[y][x] == 0)
				{
					is_tile_visible = false;
					bool b = world::has_explored(leader->world_x, leader->world_y, x, y);
					if (!b)
					{
						game::print_tile(-2000);
						continue;
					}
				}
			}

			bool skip_drawing = false;
			for (FloatingText* t : floating_text)
			{
				char ch = t->get_drawable_character(x, y);
				if (ch != 27)
				{
					std::string s;
					s.push_back(ch);
					skip_drawing = true;
					draw_text(s);
				}
			}
			if (game::wait_for_target_continue)
			{
				for (auto& coords: wait_select_coords)
				{
					if (x == coords.first && y == coords.second)
					{
						game::print_tile(254);
						skip_drawing = true;
						break;
					}
				}

			}
			if (skip_drawing)
			{
				continue;
			}
			if (is_tile_visible)
			{
				if (!game::print_character(x, y))
				{
					if (mode != "explore" || !encounters::print_encounter(x, y))
					{
						game::print_tile(game::map[y][x]);
					}
				}
			}
			else
			{
				game::print_tile(game::map[y][x]);
			}
		}
		draw_text("| \n");
	}

	for (int x = game::camera_x; x < game::camera_width + game::camera_x + 2; x++)
	{
		draw_text("-");
	}
	draw_text("\n");
}

void clear_screen()
{
	// rows of map + position text + lines for log messages
	int numlines = GAME_DISPLAY_MAP_ROWS + 1 + NUMLOGLINES;
	std::cout << "\033[" << numlines << "A\r"; // put print cursor at the top-left of the screen
}

void set_camera(Character* ch)
{
	set_camera(ch->x, ch->y);
}

void set_camera(int x, int y)
{
	game::camera_x = x - game::camera_width / 2;
	game::camera_y = y - game::camera_height / 2;

	if (game::camera_x < 0)
	{
		game::camera_x = 0;
	}
	if (game::camera_y < 0)
	{
		game::camera_y = 0;
	}
	if (game::camera_x + game::camera_width >= GAME_XMAPSIZE)
	{
		game::camera_x = GAME_XMAPSIZE - game::camera_width;
	}
	if (game::camera_y + game::camera_height >= GAME_YMAPSIZE)
	{
		game::camera_y = GAME_YMAPSIZE - game::camera_height;
	}
}

Character* get_character_by_name(std::string name)
{
	for (int i = 0; i < GAME_MAX_CHARACTERS; i++)
	{
		Character* c = game::active_characters[i];
		if (c != NULL && c->name == name)
		{
			return c;
		}
	}
	return NULL;
}

Character* get_character_nearby(int x, int y, int n) {
	std::vector<std::pair<int,int>> offsets = {
		std::make_pair(0, -1),
		std::make_pair(1, 0),
		std::make_pair(0, 1),
		std::make_pair(-1, 0),
		std::make_pair(-1, -1),
		std::make_pair(1, -1),
		std::make_pair(1, 1),
		std::make_pair(-1, 1),
	};
	std::map<int,bool> skip_mapping;

	for (unsigned int i = 0; i < offsets.size(); i++)
	{
		skip_mapping[i] = false;
	}

	if (n <= 3)
	{
		for (int i = 1; i <= n; i++)
		{
			for (unsigned int j = 0; j < offsets.size(); j++)
			{
				if (skip_mapping[j])
					continue;
				auto& offset = offsets[j];
				int xx = x + offset.first * i;
				int yy = y + offset.second * i;
				if(world::blocks_sight(xx, yy, game::map))
				{
					skip_mapping[j] = true;
					continue;
				}
				if (yy >= 0 && xx >= 0 && yy < GAME_YMAPSIZE && xx < GAME_XMAPSIZE)
				{
					Character* ch2 = game::get_character_at(xx, yy);
					if(ch2 != NULL)
					{
						return ch2;
					}
				}
			}
		}
	}
	else
	{
		for (int i = -n; i <= n; i++)
		{
			for (int j = -n; j <= n; j++)
			{
				int xx = x + j;
				int yy = y + i;
				if (yy >= 0 && xx >= 0 && yy < GAME_YMAPSIZE && xx < GAME_XMAPSIZE)
				{
					Character* ch2 = game::get_character_at(xx, yy);
					if(ch2 != NULL)
					{
						return ch2;
					}
				}
			}
		}
	}
	return NULL;
}

Character* get_character_nearby(Character* ch, int n)
{
	return get_character_nearby(ch->x, ch->y, n);
}

Character* get_character_at(int x, int y)
{
	for (int i = 0; i < GAME_MAX_CHARACTERS; i++)
	{
		Character* c = game::active_characters[i];
		if (c != NULL && c->x == x && c->y == y )
		{
			if (c->world_x == current_world_x && c->world_y == current_world_y)
			{
				return c;
			}
			else if (c->world_x == encounters::world_x && c->world_y == encounters::world_y)
			{
				return c;
			}
		}
	}
	return NULL;
}

Tile* get_tile_at(int x, int y)
{
	if (x >= 0 && x < GAME_XMAPSIZE && y >= 0 && y < GAME_YMAPSIZE)
	{
		int id = map[y][x];
		return tiles[id];
	}
	else
	{
		return tiles[0];
	}
}

int get_tile_id_at(int x, int y, int i, int j)
{
	return world::world_map[y][x][j][i];
}

void set_tile(int id, int x, int y)
{
	world::set_tile_id(id, game::current_world_x, game::current_world_y, x, y);
}
void set_tile(int id, int world_x, int world_y, int x, int y)
{
	world::set_tile_id(id, world_x, world_y, x, y);
}

void game_over()
{
	game::autodraw_enabled = false;
	game::running = false;
	log("GAME OVER");
	// wait_for_enter( [](){
	// 	game::running = false;
	// });
}

void to_csv_4d(std::stringstream& ss, int arr[GAME_WORLDSIZE][GAME_WORLDSIZE][GAME_YMAPSIZE][GAME_XMAPSIZE])
{
	for (int y = 0; y < GAME_WORLDSIZE; y++)
	{
		for (int x = 0; x < GAME_WORLDSIZE; x++)
		{
			for (int i = 0; i < GAME_YMAPSIZE; i++)
			{
				for (int j = 0; j < GAME_XMAPSIZE; j++)
				{
					int id = arr[y][x][i][j];
					ss << id << ",";
				}
			}
		}
	}
}

void save(std::string save_name)
{
	try {

	log("Saving Save/" + save_name + ".sav...");
	std::ofstream ofs("Save/" + save_name + ".sav");

	std::stringstream save;
	save << "world:";
	for (auto it: world::changed_tiles)
	{
		save << it.first << "=" << it.second << "|";
	}
	save << "\n";

	save << "characters:";
	for (int i = 0; i < CHARACTER_LIST_LENGTH; i++)
	{
		Character* ch = get_character(i);
		if (ch != NULL)
		{
			if (ch->is_cpu)
			{
				save << ch->save();
			}
			else
			{
				save << ch->save_player_character();
			}
			save << "|";
		}
	}
	save << "\n";

	save << "encounters:";
	for (auto it: encounters::active_encounters)
	{
		save << it.first << " ";
		for (auto enc: it.second)
		{
			save << enc->save() << "|";
		}
	}
	save << "\n";

	save << "player_data:";
	save << player::save_player_data();
	save << "\n";

	save << "item_storage:";
	save << player::save_item_storage();
	save << "\n";

	save << "chests:";
	for (auto it: triggers::chests)
	{
		save << it.second->save() << "|";
	} \
	save << "\n";

	save << "fog:";
	to_csv_4d(save, world::fog_of_war);
	save << "\n";

	ofs << save.rdbuf();
	ofs.close();

	log(" Saved.");

	}
	catch (std::exception& e)
	{
		throw std::runtime_error((std::string("Error loading save: ") + e.what()).c_str());
	}
}

void load(std::string save_name)
{
	try {

	log("Loading Save/" + save_name + ".sav...");
	std::fstream fs("Save/" + save_name + ".sav");

	init();

	std::string line;
	while (std::getline(fs, line))
	{
		int i = strutil::indexOf(line, ":");
		std::string name = strutil::slice(line, 0, i);
		line = strutil::slice(line, i + 1, line.size());
		if (name == "world")
		{
			std::vector<std::string> arr;
			strutil::split(line, "|", arr);
			for (std::string& entry: arr)
			{
				if (entry.size() < 2)
					continue;

				std::vector<std::string> entry_arr;
				strutil::split(entry, "=", entry_arr);

				std::vector<std::string> key_arr;
				strutil::split(entry_arr[0], ",", key_arr);

				int tile_id = std::stoi(entry_arr[1]);
				game::set_tile(
					tile_id,
					std::stoi(key_arr[0]),
					std::stoi(key_arr[1]),
					std::stoi(key_arr[2]),
					std::stoi(key_arr[3]));
			}
		}
		else if (name == "fog")
		{
			std::vector<std::string> arr;
			strutil::split(line, ",", arr);
			int ctr = 0;
			for (int y = 0; y < GAME_WORLDSIZE; y++)
			{
				for (int x = 0; x < GAME_WORLDSIZE; x++)
				{
					for (int i = 0; i < GAME_YMAPSIZE; i++)
					{
						for (int j = 0; j < GAME_XMAPSIZE; j++)
						{
							int tile_id = std::stoi(arr[ctr]);
							world::fog_of_war[y][x][i][j] = tile_id;
							ctr++;
						}
					}
				}
			}			
		}
		else if (name == "characters")
		{
			std::vector<std::string> arr;
			strutil::split(line, "|", arr);
			for (unsigned int i = 0; i < arr.size(); i++)
			{
				Character* ch = get_character(i);
				std::vector<std::string> ch_arr;
				std::vector<int> ch_arr_int;
				std::string& ch_string = arr[i];
				if (!ch_string.size())
				{
					continue;
				}
				strutil::split(ch_string, ",", ch_arr);
				for (int i = 0; i < std::min(20, (int)ch_arr.size()); i++)
				{
					ch_arr_int.push_back(std::stoi(ch_arr[i]));
				}
				ch->set(ch_arr_int[2], ch_arr_int[3], ch_arr_int[0], ch_arr_int[1]);
				if (i < 5)
				{
					ch->max_ap = ch_arr_int[4];
					ch->ap = ch_arr_int[5];
					ch->max_hp = ch_arr_int[6];
					ch->hp = ch_arr_int[7];
					ch->armor_class = ch_arr_int[8];
					ch->bonus_armor_class = ch_arr_int[9];
					ch->attack = ch_arr_int[10];
					ch->bonus_attack = ch_arr_int[11];
					ch->max_damage = ch_arr_int[12];
					ch->bonus_max_damage = ch_arr_int[13];
					ch->dmg_variance_percent = ch_arr_int[14];
					ch->magic = ch_arr_int[15];
					ch->bonus_magic = ch_arr_int[16];
					ch->damage_reduction = ch_arr_int[17];
					ch->bonus_damage_reduction = ch_arr_int[18];
					ch->effect_damage_reduction = ch_arr_int[19];

					std::string ch_name = ch_arr[22];
					std::vector<std::string> vec;

					strutil::split(ch_arr[21], ":", vec);
					ch->spells.clear();
					for (std::string& spell_name: vec)
					{
						if (spell_name.size() < 2)
						{
							continue;
						}
						ch->spells.push_back(spell_name);
					}

					vec.clear();
					strutil::split(ch_arr[20], ":", vec);
					ch->items.clear();
					for (std::string& item_name: vec)
					{
						if (item_name.size() < 2)
						{
							continue;
						}
						ch->items.push_back(item_name);
					}
				}
			}
		}
		else if (name == "chests")
		{
			std::vector<std::string> arr;
			strutil::split(line, "|", arr);
			for (std::string& chest: arr)
			{
				if (chest.size() < 2)
					continue;

				std::vector<std::string> chest_arr;
				strutil::split(chest, "=", chest_arr);

				std::string chest_key = chest_arr[0];
				std::vector<std::string> item_arr;
				strutil::split(chest_arr[1], ",", item_arr);

				auto chest_ptr = triggers::chests[chest_key];
				chest_ptr->item_names.clear();

				for (std::string& item_name: item_arr)
				{
					if (item_name.size() < 2)
						continue;
					chest_ptr->item_names.push_back(item_name);
				}
			}		
		}
		else if (name == "item_storage")
		{
			player::item_storage.clear();
			std::vector<std::string> arr;
			strutil::split(line, ",", arr);
			for (std::string& item_name: arr)
			{
				if (item_name.size() < 2)
					continue;
				player::store_item(item_name, false);
			}
		}
		else if (name == "player_data")
		{
			player::item_storage.clear();
			std::vector<std::string> arr;
			strutil::split(line, ",", arr);
			for (std::string& prop: arr)
			{
				if (prop.size() < 2)
					continue;

				std::vector<std::string> prop_arr;
				strutil::split(prop, "=", prop_arr);

				if (prop_arr[0].size() < 2)
					continue;

				player::set(prop_arr[0], prop_arr[1]);
			}		
		}

		log("LOADED TYPE: " + name);
	}

	Character* player = player::get_leader();
	game::change_map(player);

	}
	catch (std::exception& e)
	{
		throw std::runtime_error((std::string("Error loading save: '" + save_name + "' - ") + e.what()).c_str());
	}
}

bool handle_keypress(char key)
{
	int code = (int)key;
	if (code == 73) //pgup
	{
		modify_log_offset(1);
		game::clear_screen();
		game::draw();
		return false;
	}
	else if (code == 81) //pgdwn
	{
		modify_log_offset(-1);
		game::clear_screen();
		game::draw();
		return false;
	}
	if (game::wait_for_continue)
	{
		if (code == 13) // enter
		{
			game::wait_for_continue = false;
			game::wait_allow_tab = false;
			game::wait_allow_esc = false;
			game::wait_cb();
			if (!game::is_waiting())
			{
				log(" Ready.");
				log_special("");
				game::clear_screen();
				game::draw();
				return false;
			}
		}
		else
		{
			return false;
		}
	}
	else if (game::wait_for_choice_continue)
	{
		if ((code >= 49 && code <= 57) || code == 27) // 1 - 9 OR ESC
		{
			int choice = code - 48;
			if (code == 27)
			{
				if (game::wait_allow_esc)
					choice = 0;
				else
					return false;
			}
			if (choice > game::num_wait_choices)
			{
				return false;
			}
			game::wait_for_choice_continue = false;
			game::wait_allow_tab = false;
			game::wait_allow_esc = false;
			game::wait_choice_cb(choice);
			if (!game::is_waiting())
			{
				log(" Ready.");
				log_special("");
				game::clear_screen();
				game::draw();
				return false;
			}
		}
		else if (game::wait_allow_tab && code == 9)
		{
			game::wait_choice_cb(99);
		}
		else
		{
			return false;
		}
	}
	else if (game::wait_for_arrow_continue)
	{
		std::string d = "z";
		if (code == 72 || code == 56) // up
		{
			d = "n";
		}
		else if (code == 80 || code == 50) // down
		{
			d = "s";
		}
		else if (code == 75 || code == 52) // left
		{
			d = "w";
		}
		else if (code == 77 || code == 54) // right
		{
			d = "e";
		}
		else if (code == 55) // nw (num7)
		{
			d = "nw";
		}
		else if (code == 57) // ne (num9)
		{
			d = "ne";
		}
		else if (code == 51) // se (num3)
		{
			d = "se";
		}
		else if (code == 49) // sw (num1)
		{
			d = "sw";
		}

		if (d != "z")
		{
			game::wait_for_arrow_continue = false;
			game::wait_allow_tab = false;
			game::wait_allow_esc = false;
			game::wait_arrow_cb(d);
			if (!game::is_waiting())
			{
				log(" Ready.");
				log_special("");
			}
		}
	}
	else if (game::wait_for_target_continue)
	{
		if (code == 72 || code == 56) // up
		{
			wait_select_y--;
			limit_bounds(&wait_select_x, &wait_select_y, GAME_XMAPSIZE, GAME_YMAPSIZE);
			set_camera(wait_select_x, wait_select_y);
			_populate_targetting_coords();
		}
		else if (code == 80 || code == 50) // down
		{
			wait_select_y++;
			limit_bounds(&wait_select_x, &wait_select_y, GAME_XMAPSIZE, GAME_YMAPSIZE);
			set_camera(wait_select_x, wait_select_y);
			_populate_targetting_coords();
		}
		else if (code == 75 || code == 52) // left
		{
			wait_select_x--;
			limit_bounds(&wait_select_x, &wait_select_y, GAME_XMAPSIZE, GAME_YMAPSIZE);
			set_camera(wait_select_x, wait_select_y);
			_populate_targetting_coords();
		}
		else if (code == 77 || code == 54) // right
		{
			wait_select_x++;
			limit_bounds(&wait_select_x, &wait_select_y, GAME_XMAPSIZE, GAME_YMAPSIZE);
			set_camera(wait_select_x, wait_select_y);
			_populate_targetting_coords();
		}
		else if (code == 55) // nw (num7)
		{
			wait_select_x--;
			wait_select_y--;
			limit_bounds(&wait_select_x, &wait_select_y, GAME_XMAPSIZE, GAME_YMAPSIZE);
			set_camera(wait_select_x, wait_select_y);
			_populate_targetting_coords();
		}
		else if (code == 57) // ne (num9)
		{
			wait_select_x++;
			wait_select_y--;
			limit_bounds(&wait_select_x, &wait_select_y, GAME_XMAPSIZE, GAME_YMAPSIZE);
			set_camera(wait_select_x, wait_select_y);
			_populate_targetting_coords();
		}
		else if (code == 51) // se (num3)
		{
			wait_select_x++;
			wait_select_y++;
			limit_bounds(&wait_select_x, &wait_select_y, GAME_XMAPSIZE, GAME_YMAPSIZE);
			set_camera(wait_select_x, wait_select_y);
			_populate_targetting_coords();
		}
		else if (code == 49) // sw (num1)
		{
			wait_select_x--;
			wait_select_y++;
			limit_bounds(&wait_select_x, &wait_select_y, GAME_XMAPSIZE, GAME_YMAPSIZE);
			set_camera(wait_select_x, wait_select_y);
			_populate_targetting_coords();
		}
		else if (code == 13) // enter
		{
			game::wait_for_target_continue = false;
			game::wait_allow_tab = false;
			game::wait_allow_esc = false;
			game::wait_target_cb(wait_select_x, wait_select_y);
			if (!game::is_waiting())
			{
				log_special("");
			}
		}
		else if (code == 27)
		{
			game::wait_for_target_continue = false;
			log(" Cancelled targeting.");
			log_special("");
			if (game::wait_allow_esc)
			{
				game::wait_target_cb(-1,-1);
			}
			game::wait_allow_tab = false;
			game::wait_allow_esc = false;
		}
	}
	else
	{
		if (mode == "explore")
		{
			if (!explore::handle_keypress(key))
			{
				game::clear_screen();
				game::draw();
				return false;
			}
		}
		else if (mode == "combat")
		{
			combat::handle_keypress(key);
		}
	}

	return true;
}

bool update()
{
	bool should_draw = true;
	if (game::action_queue_seq.size())
	{
		should_draw = game::call_action_seq();
	}

	if (!is_waiting())
	{
		if (mode == "explore")
		{
			explore::update();
		}
		else if (mode == "combat")
		{
			combat::update();
		}
	}
	return should_draw;
}

void draw()
{
	Character* player = player::get_leader();
	if (mode == "explore")
	{
		world::set_visibility(player->x, player->y, game::map, game::vismap);
		world::assign_fog_of_war(player->world_x, player->world_y, game::vismap);
		game::print_map(true);
		player::print_status_bar();
	}
	else if (mode == "combat")
	{
		game::print_map(false);
		player::print_status_bar();
	}
	print_logs();
	flush_string();
}
};
