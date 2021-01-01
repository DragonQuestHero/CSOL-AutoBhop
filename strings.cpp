#include "client.h"

string getfilename(string path)
{
	path = path.substr(path.find_last_of("/\\") + 1);
	size_t dot_i = path.find_last_of('.');
	return path.substr(0, dot_i);
}

string get_str_between_two_str(string s, string start_delim, string stop_delim)
{
	size_t first_delim_pos = s.find_first_of(start_delim);
	size_t end_pos_of_first_delim = first_delim_pos + start_delim.length();
	size_t last_delim_pos = s.find_last_of(stop_delim);
	return s.substr(end_pos_of_first_delim, last_delim_pos - end_pos_of_first_delim);
}