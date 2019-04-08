
#ifdef __MINGW32__
#include "Windows.h"
#include <conio.h>
#include <wchar.h>
#else
#include <time.h>
#include <stdio.h>
#include <termio.h>
#include <curses.h>
#endif

#include <cstdlib>
#include <ctime>
#include <functional>
#include <iostream>

#include "Game.h"
#include "Player.h"
#include "Characters.h"
#include "DrawText.h"
#include "Combat.h"
#include "Encounters.h"
#include "Explore.h"

#ifdef __MINGW32__
#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif
int tty_init()
{
    // Set output mode to handle virtual terminal sequences
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE)
    {
        return 1;
    }

    DWORD dwMode = 0;
    if (!GetConsoleMode(hOut, &dwMode))
    {
        return 1;
    }

    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    if (!SetConsoleMode(hOut, dwMode))
    {
        return 1;
    }
    return 0;
}

int tty_break()
{
	return 0;
}

int tty_getchar()
{
	return getch();
}

int tty_fix()
{
	return 0;
}
void local_sleep()
{
	Sleep(100);
}

#else

int tty_init()
{
	return 0;
}
int tty_break()
{
	initscr();                  // enable curses
	cbreak();
	nonl();                     // don't translate newlines
	intrflush(stdscr, FALSE);   // don't process interrupt keys (like Ctrl-C)
	(void)keypad(stdscr, TRUE); // read extended keys (like arrow keys)
	return 0;
}

int tty_getchar()
{
	return getch();
}

int tty_fix()
{
	endwin();
	return 0;
}
void local_sleep()
{
	float milliseconds = 100;
	struct timespec ts;
	ts.tv_sec = milliseconds / 1000.0;
	ts.tv_nsec = ((int)milliseconds % 1000) * 1000000;
	nanosleep(&ts, NULL);
}

#endif

int main()
{
	std::cout << "Sadelica" << std::endl;

	std::srand(std::time(0));

	log("Welcome to Sadelica!");

	open_debug_log();
	tty_init();

	game::init();

	Character* player = player::get_leader();
	game::change_map(player);

	if(tty_break() != 0)
		return 1;
	tty_fix();
	game::set_camera(player);
	game::draw();
	encounters::disable_encounters = true;

	// debug lines for starting off with encounters or for talking
	// game::encounter_name = "heal_cave_pink";
	// game::set_mode("combat");

	// Character* ch = get_character("Tomb Apparition");
	// game::add_character_to_current_map(ch, 10, 3);
	// explore::talk(ch);

	char key_press = 'c';
	int code = -1;

	int frame = 0;
	while (game::running)
	{
		if (game::autodraw_enabled)
		{
			while (game::action_queue_seq.size())
			{
				frame++;
				local_sleep();
				if (game::update())
				{
					game::clear_screen();
					game::draw();
				}
			}
			continue;
		}

		if (game::input_is_enabled)
		{
			if(tty_break() != 0)
				return 1;
			key_press = tty_getchar();
			code = (int)key_press;
			tty_fix();
		}
		else
		{
			key_press = 'z';
			code = 97;
			local_sleep();
		}

		if (key_press == 'q') // 'q'
		{
			break;
		}
		if (code < 0) // skip invalid keys
		{
			continue;
		}

		frame++;

		if (game::handle_keypress(key_press))
		{
			game::update();
			game::clear_screen();
			game::draw();
		}
	}

	close_debug_log();
	std::cout << "Sadelica over." << std::endl;

	return 0;
}
