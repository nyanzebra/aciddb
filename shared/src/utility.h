#pragma once

#include <boost/format.hpp>

inline std::string FormatImpl(boost::format& formatter) {
	return formatter.str();
}

template<typename Head, typename... Tail>
std::string FormatImpl(boost::format& formatter, Head&& h, Tail&& ... args) {
	formatter % std::forward<Head>(h);
	return FormatImpl(formatter, std::forward<Tail>(args)...);
}

/**
* Variadic wrapper for boost.format.
*/
template<typename... Args>
std::string Format(std::string fmt, Args&& ... args) {
	boost::format message(fmt);
	try {
		return FormatImpl(message, std::forward<Args>(args)...);
	} catch (...) {}
	return std::string("[invalid format]: ") + fmt;
}

std::vector<std::string>& Split(const std::string &str, char delimiter, std::vector<std::string> &dest);
std::vector<std::string> Split(const std::string &str, char delimiter);