#include "utility.h"
#include <sstream>
#include <vector>

std::vector<std::string> &Split(const std::string &str, char delimiter, std::vector<std::string> &dest) {
	std::stringstream ss(str);
	std::string item;
	while(std::getline(ss, item, delimiter)) {
		dest.push_back(item);
	}
	return dest;
}

std::vector<std::string> Split(const std::string &str, char delimiter) {
	std::vector<std::string> out;
	return Split(str, delimiter, out);
}