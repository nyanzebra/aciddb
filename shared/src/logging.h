#pragma once

#include <iostream>
#include <mutex>

#include "utility.h"

#define DEFAULT_LOG_LEVEL kLogLevelDebug

enum LogLevel {
	kLogLevelDebug  = 0,
	kLogLevelInfo   = 1,
	kLogLevelWarn   = 2,
	kLogLevelError  = 3,
	kLogLevelAlways = 100,
};

const char* LogLevelStr(LogLevel level);

namespace LoggingImpl {
	extern LogLevel _gLevel;
	extern std::mutex gLoggingMutex;
}

void SetLoggingLevel(LogLevel level);

template <typename ... Args>
void Logf(std::basic_ostream<char>& out, LogLevel level, const std::string& format, Args&& ... args) {
	std::lock_guard<std::mutex> l(LoggingImpl::gLoggingMutex);
	if (level >= LoggingImpl::_gLevel) {
		out << "[" << LogLevelStr(level) << "] " << Format(format, std::forward<Args>(args)...) << std::endl;	
	}
}

template <typename ... Args>
void Logf(std::basic_ostream<char>& out, const std::string& format, Args&& ... args) {
	std::lock_guard<std::mutex> l(LoggingImpl::gLoggingMutex);
	out << Format(format, std::forward<Args>(args)...) << std::endl;
}

template <typename ... Args>
void Logf(const std::string& format, Args&& ... args) {
	Logf(std::cout, format, std::forward<Args>(args)...);
}

template <typename ... Args>
void Logf(LogLevel level, const std::string& format, Args&& ... args) {
	Logf(std::cout, level, format, std::forward<Args>(args)...);
}