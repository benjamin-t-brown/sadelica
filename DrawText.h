#pragma once

#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include <boost/algorithm/string.hpp>
#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/binary_from_base64.hpp>
#include <boost/archive/iterators/transform_width.hpp>

extern int draw_text_log_offset;
int get_number_from_color(std::string color);
std::string get_text(std::string text, std::string color = "", std::string bgcolor = "");
void modify_log_offset(int i);

#define NUMLOGLINES 9
#define MAXLOGWIDTH 80

void draw_text(std::string str);
void draw_text(std::stringstream& ss);

void open_debug_log();
void log_debug(std::string str);
void close_debug_log();

void log(std::string text);
void log_dialog(std::string text);
void log_special(std::string text = "");
void print_logs();
void flush_string();

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

namespace strutil
{
static inline void split(const std::string& str, const std::string& token, std::vector<std::string>& out);
static inline std::string join(const std::vector<std::string>& arr, const std::string& token);
static inline int indexOf(const std::string& str, const std::string& token);
static inline int lastIndexOf(const std::string& str, const std::string& token);
static inline std::string slice(const std::string& str, int a, int b);
static inline std::string remv(const std::string& str, int i, int n);
static inline std::string reverse(const std::string& str);
static inline void wrap(const std::string& str, int width, int text_size, std::vector<std::string>& out);
static inline std::string trim(const std::string& str);
static inline bool compareFilename(const std::string& str1, const std::string& str2);
// lol thanks https://stackoverflow.com/questions/7053538/how-do-i-encode-a-string-to-base64-using-only-boost
static inline std::string decode64(const std::string& val);
static inline std::string encode64(const std::string& val);
}

namespace strutil
{
//split a string 'str' at each token 'token' and put the results into vector 'out'
static inline void split(const std::string& str, const std::string& token, std::vector<std::string>& out)
{
	if (str.size() == 0)
	{
		unsigned int len = str.size();
		for (unsigned int i = 0; i < len; ++i)
		{
			std::string s = std::to_string(str.at(i));
			out.push_back(s);
		}
		return;
	}

	bool searching = true;
	std::size_t first_index = 0;
	std::size_t second_index = 0;
	while (searching)
	{
		second_index = str.find(token, first_index);
		if (second_index == std::string::npos || second_index < first_index)
		{
			searching = false;
		}
		else
		{
			out.push_back(strutil::slice(str, first_index, second_index));
			first_index = second_index + 1;
		}
	}
	if (second_index == 0)
	{
		out.push_back(str);
	}
	else
	{
		out.push_back(strutil::slice(str, first_index, str.size()));
	}
}

// join a vector 'arr' by inserting token 'token' in between each element, and return
// the resulting string
static inline std::string join(const std::vector<std::string>& arr, const std::string& token)
{
	std::string ret("");
	unsigned int len = arr.size();
	for (unsigned int i = 0; i < len; ++i)
	{
		if (i == len - 1)
		{
			ret += arr[i];
		}
		else
		{
			ret += arr[i] + token;
		}
	}
	return ret;
}

// get the first index of 'token' inside string 'str', or -1 if not found
static inline int indexOf(const std::string& str, const std::string& token)
{
	std::size_t i = str.find(token, 0);
	if (i == std::string::npos)
	{
		return -1;
	}
	else
	{
		return (int)i;
	}
}

// get the last index of 'token' inside string 'str', or -1 if not found
static inline int lastIndexOf(const std::string& str, const std::string& token)
{
	std::string rev = strutil::reverse(str);
	int i = strutil::indexOf(rev, token);
	if (i == -1)
	{
		return i;
	}
	else
	{
		return (int)str.size() - 1 - i;
	}
}

// return a substring of 'str' between index 'a' and index 'b', including a.  If 'b' is negative or 0,
// treat it as an offset from the end.
//    ex: slice( "abcdefg", 0, -1 ) == "abcdef"
static inline std::string slice(const std::string& str, int a, int b)
{
	if (a == b)
	{
		return std::string("");
	}

	if (b <= 0)
	{
		b = str.size() + b;
	}
	else if ((unsigned int)b > str.size())
	{
		b = str.size();
	}

	if (a < 0)
	{
		a = str.size() + a;
	}
	else if ((unsigned int)a > str.size())
	{
		a = str.size();
	}

	if (b <= a || a < 0)
	{
		return std::string("");
	}

	return str.substr(a, b - a);
}

//remove 'n' characters from string 'str' beginning at index 'i'
static inline std::string remv(const std::string& str, int i, int n)
{
	return strutil::slice(str, 0, i) + strutil::slice(str, i + n + 1, 0);
}

// reverse a string, returning a new string
static inline std::string reverse(const std::string& str)
{
	std::string ret = std::string(str);
	std::reverse(ret.begin(), ret.end());
	return ret;
}

// given a string, split it into lines based on a maximum width in pixels and the size of the text ( 10, 20, or 40 )
static inline void wrap(const std::string& str, int width, int text_size, std::vector<std::string>& out)
{
	std::vector<std::string> words;
	strutil::split(str, " ", words);

	float text_width = 0;
	float max_width = 0;
	int chk_width = width;
	int num_lines = 0;
	std::string line = std::string("");
	std::string current_line = std::string("");

	for (std::string word : words)
	{
		std::string current_line;
		current_line = line + std::string(" ") + word;
		text_width = current_line.size() * ((float)text_size / 1.5f);
		if (text_width > chk_width)
		{
			float lnw = line.size() * ((float)text_size / 1.5f);
			if (line.size())
			{
				if (lnw > max_width)
				{
					max_width = lnw;
				}
				num_lines++;
				out.push_back(strutil::trim(line));
			}
			line = std::string(word);
		}
		else
		{
			line = std::string(current_line);
		}
	}
	if (line.size())
	{
		float lnw = line.size() * ((float)text_size / 1.5f);
		if (lnw > max_width)
		{
			max_width = lnw;
		}
		num_lines++;
		out.push_back(strutil::trim(line));
	}
}

// remove all the space characters from the beginning and end of a string
static inline std::string trim(const std::string& str)
{
	if (!str.size())
	{
		return std::string(str);
	}

	std::string ret(str);
	int first_nonspace = 0;
	int last_nonspace = str.size() - 1;
	for (unsigned int i = 0; i < str.size(); i++)
	{
		if (str[i] != ' ')
		{
			first_nonspace = i;
			break;
		}
	}

	for (unsigned int i = str.size() - 1; i >= 0; i--)
	{
		if (str[i] != ' ')
		{
			last_nonspace = i;
			break;
		}
	}
	if (first_nonspace == last_nonspace)
	{
		return std::to_string(str[first_nonspace]);
	}
	else
	{
		return strutil::slice(str, first_nonspace, last_nonspace + 1);
	}
}

//compare two strings up to the last "." in them
static inline bool compareFilename(const std::string& str1, const std::string& str2)
{
	int str1_ind = strutil::lastIndexOf(str1, ".");
	int str2_ind = strutil::lastIndexOf(str2, ".");

	std::string a;
	std::string b;

	if (str1_ind >= 0)
	{
		a = strutil::slice(str1, 0, str1_ind);
	}
	else
	{
		a = str1;
	}

	if (str2_ind >= 0)
	{
		b = strutil::slice(str1, 0, str2_ind);
	}
	else
	{
		b = str2;
	}

	return a == b;
}
static inline std::string decode64(const std::string& val)
{
	using namespace boost::archive::iterators;
	using It = transform_width<binary_from_base64<std::string::const_iterator>, 8, 6>;
	return boost::algorithm::trim_right_copy_if(std::string(It(std::begin(val)), It(std::end(val))), [](char c) { return c == '\0'; });
}
static inline std::string encode64(const std::string& val)
{
	using namespace boost::archive::iterators;
	using It = base64_from_binary<transform_width<std::string::const_iterator, 6, 8>>;
	auto tmp = std::string(It(std::begin(val)), It(std::end(val)));
	return tmp.append((3 - val.size() % 3) % 3, '=');
}
};
