#include <testing.h>

#include "../src/RecordEvent.h"
#include "../src/Datastore.h"

TEST_SUITE("RecordEvents") {

	TEST("Set") {
		Datastore ds;

		{
			EventSet e("", "test data"); // invalid key
			CHECK(!e(&ds));
			CHECK(strcmp(ds.getValue(""), "test data"));
		}

		{
			EventSet e("child 1", "some key");
			CHECK(e(&ds));
			CHECK(!strcmp(ds.getValue("child 1"), "some key"));
		}

		{
			EventSet e("child 1", "some new key"); // overwrite previous value
			CHECK(e(&ds));
			CHECK(!strcmp(ds.getValue("child 1"), "some new key"));
		}

		{
			EventSet e("child 1:child 2", "some other key"); // child 1 is a string record, can't have children
			CHECK(!e(&ds));
			CHECK(strcmp(ds.getValue("child 1:child 2"), "some other key"));
		}

		{
			EventSet e("child 2:child 3:child 4", "nested record creation");
			CHECK(e(&ds));
			CHECK(!strcmp(ds.getValue("child 2:child 3:child 4"), "nested record creation"));
		}

		CHECK(!strcmp(ds.getValue("child 1"), "some new key"));
		CHECK(!strcmp(ds.getValue("child 2:child 3:child 4"), "nested record creation"));
	};
};