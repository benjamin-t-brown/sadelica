#include "Explore.h"
#include "Encounters.h"
#include "Game.h"
#include "Player.h"
#include "Triggers.h"
#include "World.h"
#include "Characters.h"

namespace explore
{
bool end_conversation = false;
bool _end_conversation_from_ch = false;

void move_npc(Character* ch, int x_offset, int y_offset)
{
	Character* ch2 = game::get_character_at(ch->x + x_offset, ch->y + y_offset);
	if (ch2 != NULL)
	{
		return;
	}

	ch->old_x = ch->x;
	ch->old_y = ch->y;
	ch->x += x_offset;
	ch->y += y_offset;

	if (ch->x < 0)
	{
		ch->x = 0;
	}
	else if (ch->x >= GAME_XMAPSIZE)
	{
		ch->x = GAME_XMAPSIZE - 1;
	}
	else if (ch->y < 0)
	{
		ch->y = 0;
	}
	else if (ch->y >= GAME_YMAPSIZE)
	{
		ch->y = GAME_YMAPSIZE - 1;
	}
	else if (
		game::map[ch->y][ch->x] >= GAME_MOVE_THRESHOLD ||
		game::map[ch->y][ch->x] < 0 ||
		world::is_open_door(game::map[ch->y][ch->x]))
	{
		ch->x = ch->old_x;
		ch->y = ch->old_y;
	}
}

void move_character(Character* ch, int x_offset, int y_offset)
{
	log("Move " + game::offset_to_direction(x_offset, y_offset) + ".");

	int new_x = ch->x + x_offset;
	int new_y = ch->y + y_offset;

	Character* ch2 = game::get_character_at(new_x, new_y);
	if (ch2 != NULL)
	{
		log(" Somebody stands where you wish to walk! ");
		return;
	}

	ch->old_x = ch->x;
	ch->old_y = ch->y;
	ch->x += x_offset;
	ch->y += y_offset;

	if (ch->x < 0)
	{
		ch->x = GAME_XMAPSIZE - 1;
		ch->world_x--;
		game::change_map(ch);
		player::move_party_members();
	}
	if (ch->x >= GAME_XMAPSIZE)
	{
		ch->x = 0;
		ch->world_x++;
		game::change_map(ch);
		player::move_party_members();
	}
	if (ch->y < 0)
	{
		ch->y = GAME_YMAPSIZE - 1;
		ch->world_y--;
		game::change_map(ch);
		player::move_party_members();
	}
	if (ch->y >= GAME_YMAPSIZE)
	{
		ch->y = 0;
		ch->world_y++;
		game::change_map(ch);
		player::move_party_members();
	}

	if (call_trigger(ch->world_x, ch->world_y, ch->x, ch->y, ch))
	{
		player::move_party_members();
	}
	else if (world::is_door(game::map[ch->y][ch->x]))
	{
		game::map[ch->y][ch->x]--;
		ch->x = ch->old_x;
		ch->y = ch->old_y;
	}
	else if (false && game::map[ch->y][ch->x] >= GAME_MOVE_THRESHOLD && !world::is_open_door(game::map[ch->y][ch->x]))
	{
		ch->x = ch->old_x;
		ch->y = ch->old_y;
	}
	else
	{
		player::move_party_members();
	}
}

void talk(Character* ch, bool force)
{
	if (force)
	{
		explore::_end_conversation_from_ch = false;
		explore::end_conversation = false;
	}

	if (explore::end_conversation)
	{
		explore::end_conversation = false;
		return;
	}

	if (ch->talk_index == 0 && ch->dialogs.size() == 0)
	{
		log(" You cannot talk to this person.");
	}
	else if (ch->talk_index < (int)ch->dialogs.size())
	{
		log("");
		ch->talk_index++;
		ch->dialogs[ch->talk_index - 1](ch);
	}
	else
	{
		ch->talk_index = ch->base_talk_index;
		return;
	}

	std::function<void()> cb = [=]() {
		explore::talk(ch);
	};

	game::wait_for_enter(cb);
}

void end_talk(Character* ch)
{
	ch->talk_index = ch->base_talk_index;
	log_special("");
	explore::end_conversation = true;
	explore::_end_conversation_from_ch = true;
}

void look()
{
	game::wait_for_arrow("Look... ",
		[](std::string& d) {
			int arr[2];
			game::direction_to_offset(d, arr);
			Character* plch = player::get_leader();
			int x = plch->x + arr[0];
			int y = plch->y + arr[1];

			if (call_trigger(plch->world_x, plch->world_y, x, y, plch))
			{
				player::move_party_members();
				return;
			}

			std::string sign_text = get_sign_text(plch->world_x, plch->world_y, x, y);
			if (sign_text.size())
			{
				log_dialog("The sign says: " + sign_text + "");
				return;
			}

			if (show_chest(plch->world_x, plch->world_y, x, y))
			{
				return;
			}

			Tile* t = game::get_tile_at(x, y);
			log(" You see: " + t->description + ".");
		});
}

bool handle_keypress(char key)
{
	bool did_action = false;
	int code = (int)key;
	Character* plch = player::get_leader();
	if (code == 72 || code == 56) // up
	{
		did_action = true;
		explore::move_character(plch, 0, -1);
	}
	else if (code == 80 || code == 50) // down
	{
		did_action = true;
		explore::move_character(plch, 0, 1);
	}
	else if (code == 75 || code == 52) // left
	{
		did_action = true;
		explore::move_character(plch, -1, 0);
	}
	else if (code == 77 || code == 54) // right
	{
		did_action = true;
		explore::move_character(plch, 1, 0);
	}
	else if (code == 55) // nw (num7)
	{
		did_action = true;
		explore::move_character(plch, -1, -1);
	}
	else if (code == 57) // ne (num9)
	{
		did_action = true;
		explore::move_character(plch, 1, -1);
	}
	else if (code == 51) // se (num3)
	{
		did_action = true;
		explore::move_character(plch, 1, 1);
	}
	else if (code == 49) // sw (num1)
	{
		did_action = true;
		explore::move_character(plch, -1, 1);
	}
	else if (code == 49) // 1
	{
		player::select_character(0);
	}
	else if (code == 50) // 2
	{
		player::select_character(1);
	}
	else if (code == 51) // 3
	{
		player::select_character(2);
	}
	else if (code == 52) // 4
	{
		player::select_character(3);
	}
	else if (code == 9) // tab
	{
		player::select_character(-1);
	}
	else if (key == '`')
	{
		Character* ch = player::get_leader();
		std::stringstream ss;
		ss << "Position"
		   << ": " << ch->world_x << "," << ch->world_y << "," << ch->x << "," << ch->y;
		log(ss.str());
		log("Tile ID: " + std::to_string(game::get_tile_id_at(ch->world_x, ch->world_y, ch->x, ch->y)));
	}
	else if (key == 'j') //for 'journal'
	{
		auto playerData = player::get_player_data();
		log("Player data: ");
		for (auto it : playerData)
		{
			std::stringstream ss;
			ss << " " << it.first << ": " << it.second;
			log(ss.str());
		}
	}
	else if (code == 'p') //for 'party'
	{
		player::log_player();
	}
	else if (code == 'b')
	{
		player::log_storage();
	}
	else if (code == 'i') // for 'inventory'
	{
		player::show_inventory_menu();
	}
	else if (code == 'S')
	{
		game::save("test");
	}
	else if (code == 'L')
	{
		game::load("test");
	}
	else if (code == 'c')
	{
		player::log_character(player::get_selected_character());
	}
	else if (code == 32) // space
	{
		log("Talk:");
		Character* ch = game::get_character_nearby(plch, 2);
		if (ch != NULL)
		{
			log("  " + ch->name);
			explore::talk(ch);
		}
		else
		{
			log(" There's nobody with which to speak!");
		}
	}
	else if (code == 'l')
	{
		look();
	}
	else
	{
		std::stringstream ss;
		ss << key << " " << code;
		log(ss.str());
	}

	Character* ch = player::get_leader();
	game::set_camera(ch);
	return did_action;
}

void update()
{
	for (int i = 0; i < GAME_MAX_CHARACTERS; i++)
	{
		Character* c = game::active_characters[i];
		if (c != NULL && !c->is_dead())
		{
			c->act();
		}
	}

	encounters::update_encounters();
}
};