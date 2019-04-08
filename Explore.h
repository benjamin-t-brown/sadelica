#pragma once

#include <string>
#include <vector>

class Character;

namespace explore
{

extern bool end_conversation;
extern bool _end_conversation_from_ch;

void move_npc(Character* ch, int x_offset, int y_offset);
void move_character(Character* ch, int x_offset, int y_offset);
void talk(Character* ch, bool force = false);
void end_talk(Character* ch);
void look();
void open_store(int store_id);
bool handle_keypress(char key);
void update();
}