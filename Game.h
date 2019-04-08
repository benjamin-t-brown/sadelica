#pragma once

#include <functional>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "DrawText.h"

#define GAME_WORLDSIZE 10
#define GAME_XMAPSIZE 25
#define GAME_YMAPSIZE 25

#define GAME_MAX_CHARACTERS 100

#define GAME_CAMERA_WIDTH 11
#define GAME_CAMERA_HEIGHT 11

#define GAME_DISPLAY_MAP_ROWS GAME_CAMERA_HEIGHT + 2

#define GAME_MOVE_THRESHOLD 99 // tile ids >= this value count as walls

#define GAME_EMPTY_FUNC []() {}

#define TO1D(x,y) y*GAME_XMAPSIZE + x
#define TO4D(x,y) x/GAME_XMAPSIZE,y/GAME_YMAPSIZE,x%GAME_XMAPSIZE,y%GAME_YMAPSIZE

class Encounter;

class Tile
{
  public:
	std::string color;
	std::string bgcolor;
	std::string text;
	std::string description;

	Tile(std::string text, std::string color, std::string bgcolor, std::string description);
	void print(bool use_colors = true);
};

class Character;

class FloatingText
{
  public:
	std::string text;
	int x;
	int y;

	FloatingText(std::string text, int x, int y);

	char get_drawable_character(int _x, int _y);
};

template<typename T, typename... U>
size_t get_pointer_address(std::function<T(U...)> f) {
    typedef T(fnType)(U...);
    fnType ** fnPointer = f.template target<fnType*>();
    return (size_t) *fnPointer;
}

namespace game
{
extern int current_world_x;
extern int current_world_y;
extern int map[GAME_YMAPSIZE][GAME_XMAPSIZE];
extern int vismap[GAME_YMAPSIZE][GAME_XMAPSIZE];
extern Character* active_characters[GAME_MAX_CHARACTERS];
extern std::map<int, Tile*> tiles;
extern bool running;

extern int camera_x;
extern int camera_y;
extern int camera_width;
extern int camera_height;

extern std::string encounter_name;

extern int wait_frame;
extern int max_wait_frame;

extern std::function<void()> wait_cb;
extern std::function<void(int)> wait_choice_cb;
extern std::function<void(std::string&)> wait_arrow_cb;
extern std::function<void(int, int)> wait_target_cb;
extern std::function<void()> on_encounter_end_cb;
extern int num_wait_choices;
extern bool wait_for_continue;
extern bool wait_for_choice_continue;
extern bool wait_for_arrow_continue;
extern bool wait_for_target_continue;
extern int wait_target_size;
extern bool wait_allow_tab;
extern bool wait_allow_esc;
extern std::vector<std::pair<int,int>> wait_select_coords;
extern int wait_select_x;
extern int wait_select_y;
extern std::vector<FloatingText*> floating_text;

extern std::string mode;
extern bool autodraw_enabled;
extern bool input_is_enabled;

extern std::vector<std::pair<int, std::function<void()>>> action_queue_seq;
extern std::vector<std::pair<int, std::function<void()>>> action_queue_pll;

void init();

void add_tile(int id, std::string text, std::string color, std::string bgcolor);
Tile* get_tile(int id);

void disable_input();
void enable_input();

std::string offset_to_direction(int x_offset, int y_offset);
void direction_to_offset(std::string d, int arr[2]);
void direction_to_offset(std::string d, int& x_offset, int& y_offset);
int get_random(int low, int high);
bool get_probability(int percent);
std::string pick_string(std::vector<std::string> arr);
void limit_bounds(int* x, int* y, int maxw, int maxh);
bool is_out_of_bounds(int x, int y);
std::string get_heading(Character* ch);
std::string get_cardinal(Character* ch);
std::vector<int> to4d(int x, int y);

void clear_characters();
void change_map(int world_x, int world_y);
void change_map(Character* ch);
void add_character_to_current_map(Character* ch, int x, int y);
void set_mode(std::string m);
void start_encounter(std::string encounter_name, std::function<void()> on_start=GAME_EMPTY_FUNC, std::function<void()> on_end=GAME_EMPTY_FUNC);

bool is_waiting();
void wait_for_enter(std::function<void()> cb = GAME_EMPTY_FUNC);
void wait_for_choice(std::string text, std::vector<std::string>& choice_text, std::function<void(int)> cb);
void wait_for_many_choices(
	std::string text,
	std::vector<std::string>& choice_text,
	std::function<void(int)> cb,
	unsigned int many_choices_start_ind = 0);
void wait_for_arrow(std::string text, std::function<void(std::string&)> cb);
void wait_for_target(std::function<void(int, int)> cb, int target_size=1);
void allow_tab_while_waiting();
void allow_esc_while_waiting();
void add_action_seq(std::function<void()> cb = GAME_EMPTY_FUNC, int frames = 0);
bool call_action_seq();
void add_floating_text(std::string text, int frames, int x, int y);
void log_table_n(std::vector<std::vector<std::string>>& columns, bool is_numbered);

void print_tile(int tile, bool use_colors = true);
bool print_character(int x, int y);
void print_map(bool use_visibility = false);
void clear_screen();

void set_camera(Character* ch);
void set_camera(int x, int y);
Character* get_character_by_name(std::string name);
Character* get_character_nearby(int x, int y, int n = 1);
Character* get_character_nearby(Character* ch, int n = 1);
Character* get_character_at(int x, int y);
Tile* get_tile_at(int x, int y);
int get_tile_id_at(int x, int y, int i, int j);
void set_tile(int id, int x, int y);
void set_tile(int id, int world_x, int world_y, int x, int y);

void game_over();
void save(std::string save_name);
void load(std::string save_name);

bool handle_keypress(char key);
bool update();
void draw();
}
