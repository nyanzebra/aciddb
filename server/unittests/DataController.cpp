#include <testing.h>

#include <sstream>

#include "../src/DataController.h"

TEST_SUITE("DataController") {

	TEST("Usage") {

		std::stringstream dsFile;
		std::stringstream jFile;

		DataController dc(dsFile, jFile);

		CHECK(dc.processTransaction({"set root:child1 key1"}) == Result({OK_STRING}));
		CHECK(dc.processTransaction({"set root:child2 key2"}) == Result({OK_STRING}));

		CHECK(dc.processTransaction({"get root:child1"}) == Result({"key1"}));
		CHECK(dc.processTransaction({"get root:child2"}) == Result({"key2"}));
		CHECK(dc.processTransaction({"get dne"}) == Result({""}));

		CHECK(dc.processTransaction({"move root:child1 root:temp:child1"}) == Result({OK_STRING}));
		CHECK(dc.processTransaction({"get root:temp:child1"}) == Result({"key1"}));
		CHECK(dc.processTransaction({"move root:child1 root:temp:child1"}) == Result({NOT_FOUND_STRING}));

		CHECK(dc.processTransaction({"delete root:child1"}) == Result({OK_STRING})); // ok even if the key was moved, nothing was deleted
		CHECK(dc.processTransaction({"delete root:child2"}) == Result({OK_STRING})); // deletes child2

		CHECK(dc.processTransaction({"get root:child2"}) == Result({""})); // nothing there
		CHECK(dc.processTransaction({"get root:temp:child1"}) == Result({"key1"}));

		// multiple statements at once

		CHECK(dc.processTransaction({
			"delete root:child1",
			"delete root:child2",
			"get root:child2",
			"get root:temp:child1"
		}) == Result({
			OK_STRING,
			OK_STRING,
			"",
			"key1"
		}));
	};
};
