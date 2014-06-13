#include "utility.h"
#include <sstream>
#include <vector>

std::vector<std::string> &Split(const std::string &str, char delimiter, std::vector<std::string> &dest) {
	std::stringstream ss(str);
	std::string item;
	std::string temp = "";
	static bool add = false;
	static bool skip = false;

	while(std::getline(ss, item, delimiter)) {
		if(add) {
			temp += " " + item;
			add = false;
			dest.push_back(temp);
			temp = "";
			skip = true;
		}

		if (item.front() == '"' && item.back() != '"') {
			temp += item;
			add = true;
		}
		else if (skip) {
			skip = false;
		}
		else {
			dest.push_back(item);
		}
	}
	return dest;
}

std::vector<std::string> Split(const std::string &str, char delimiter) {
	std::vector<std::string> out;
	return Split(str, delimiter, out);
}