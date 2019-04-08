#include "DrawText.h"

#include <fstream>

std::ofstream debug_file;
std::stringstream screen;
std::vector<std::string> logs;
std::string special = "";
int draw_text_log_offset = 0;

void _split_text(std::string text, std::vector<std::string>& arr)
{
	while ((int)(text.size()) > MAXLOGWIDTH)
	{
		int i = MAXLOGWIDTH - 1;
		while (text[i] != ' ')
		{
			i--;
		}
		std::string new_text = text.substr(0, i);
		while (new_text.size() && new_text[0] == ' ')
		{
			new_text = new_text.substr(1);
		}
		arr.push_back(new_text);
		text = text.substr(i);
	}
	std::string new_text = text;
	if (arr.size())
	{
		while (new_text.size() && new_text[0] == ' ')
		{
			new_text = new_text.substr(1);
		}
	}
	arr.push_back(new_text);
}


void open_debug_log()
{
	debug_file.open("./debug.out", std::ios_base::out | std::ios::trunc);
	log_debug("=== Sadelica Debug File ===");
}
void log_debug(std::string str)
{
	debug_file << str << std::endl;;
}
void close_debug_log()
{
	log_debug("=== End debug ===");
	debug_file.close();
}

void draw_text(std::string str)
{
	screen << str;
}
void draw_text(std::stringstream& ss)
{
	screen << ss.str();
}

int get_number_from_color(std::string color)
{
	std::map<std::string, int> colors;
	colors["black"] = 30;
	colors["red"] = 31;
	colors["green"] = 32;
	colors["yellow"] = 33;
	colors["blue"] = 34;
	colors["pink"] = 35;
	colors["cyan"] = 36;
	colors["white"] = 37;
	colors["b_black"] = 90;
	colors["b_red"] = 91;
	colors["b_green"] = 92;
	colors["b_yellow"] = 93;
	colors["b_blue"] = 94;
	colors["b_pink"] = 95;
	colors["b_cyan"] = 96;
	colors["b_white"] = 97;

	if (colors.find(color) != colors.end())
	{
		return colors[color];
	}
	else
	{
		return -1;
	}
}

std::string get_text(std::string text, std::string color, std::string bgcolor)
{
	std::stringstream ss;
	int textcolor = get_number_from_color(color);
	int backgroundcolor = get_number_from_color(bgcolor);

	ss << "\033[";
	if (textcolor > -1 && backgroundcolor > -1)
	{
		ss << textcolor << ";" << (backgroundcolor + 10) << "m" << text << "\033[0m";
	}
	else if (textcolor > -1)
	{
		ss << textcolor << "m" << text << "\033[0m";
	}
	else if (backgroundcolor > -1)
	{
		ss << (backgroundcolor + 10) << "m" << text << "\033[0m";
	}
	else
	{
		ss << "0m" << text;
	}
	return ss.str();
}

void log(std::string text)
{
	draw_text_log_offset = 0;
	std::vector<std::string> arr;
	_split_text(text, arr);
	for (std::string text : arr)
	{
		logs.push_back(text);
		log_debug(text);
	}
}

void log_dialog(std::string text)
{
	draw_text_log_offset = 0;
	std::vector<std::string> arr;
	_split_text(text, arr);
	for (std::string text : arr)
	{
		logs.push_back("    " + text);
		log_debug(text);
	}
}

void log_special(std::string text)
{
	draw_text_log_offset = 0;
	special = text + "\033[K";
	if (!text.size())
	{
		special = "\033[K";
	}
}

void modify_log_offset(int i)
{
	draw_text_log_offset += i;
	if (draw_text_log_offset < 0)
	{
		draw_text_log_offset = 0;
	}
	if (logs.size() < NUMLOGLINES)
	{
		draw_text_log_offset = 0;
	}
	else if (draw_text_log_offset > (int)logs.size() - NUMLOGLINES)
	{
		draw_text_log_offset = logs.size() - NUMLOGLINES;
	}
}

void print_logs()
{
	for (int i = logs.size() - NUMLOGLINES - draw_text_log_offset; i < (int)logs.size() - draw_text_log_offset; i++)
	{
		if (i >= 0)
		{
			//std::cout << logs[i] << "\033[K" << std::endl;
			draw_text(logs[i] + std::string("\033[K\n"));
		}
		else
		{
			//std::cout << "\033[K" << std::endl;
			draw_text("\033[K\n");
		}
	}
	//std::cout << special << " ";
	draw_text((special + " ").c_str());
}

void flush_string()
{
	std::cout << screen.str();
	screen.str("");
}