#include "logging.h"

namespace LoggingImpl {
	LogLevel _gLevel = DEFAULT_LOG_LEVEL;
}

void SetLoggingLevel(LogLevel level) {
	LoggingImpl::_gLevel = level;
}

const char* LogLevelStr(LogLevel level) {
	switch(level) {
		case kLogLevelDebug:
			return "Debug";
		case kLogLevelInfo:
			return "Info";
		case kLogLevelWarn:
			return "Warn";
		case kLogLevelError:
			return "Error";
		case kLogLevelAlways: // surpress warning
			return "";
	}
	return "";
}
