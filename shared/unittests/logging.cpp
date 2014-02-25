#include <sstream>

#include <testing.h>

#include "../src/logging.h"

TEST_SUITE("Logging") {

	TEST("Usage") {

		std::stringstream ss;

		SetLoggingLevel(kLogLevelInfo);

		Logf(ss, kLogLevelDebug, "test");
		CHECK(ss.str().empty());

		Logf(ss, kLogLevelInfo, "test");
		CHECK(ss.str() == "[Info] test\n");
		ss.str("");

		Logf(ss, kLogLevelWarn, "test");
		CHECK(ss.str() == "[Warn] test\n");
		ss.str("");

		Logf(ss, kLogLevelError, "test");
		CHECK(ss.str() == "[Error] test\n");
		ss.str("");

		SetLoggingLevel(kLogLevelError);

		Logf(ss, kLogLevelDebug, "test");
		CHECK(ss.str().empty());

		Logf(ss, kLogLevelInfo, "test");
		CHECK(ss.str().empty());

		Logf(ss, kLogLevelWarn, "test");
		CHECK(ss.str().empty());

		Logf(ss, kLogLevelError, "test");
		CHECK(ss.str() == "[Error] test\n");
		ss.str("");

		SetLoggingLevel(kLogLevelError);

		Logf(ss, "test");
		CHECK(ss.str() == "test\n");
		ss.str("");

	};

};