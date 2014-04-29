#include <testing.h>

#include "../src/RecordEvent.h"
#include "../src/Datastore.h"

TEST_SUITE("RecordEvents") {

	TEST("Delete") {

		Datastore ds;

		EventSet("child 1", "some new key")(&ds);
		EventSet("child 2:child 3:child 4", "nested record creation")(&ds);
		EventSet("child 2:child 5", "child 5 record")(&ds);

		{
			EventDelete e(""); // invalid key
			CHECK(e(&ds));
		}

		// data should still exist
		CHECK(ds.getValue("child 1") == "some new key");
		CHECK(ds.getValue("child 2:child 3:child 4") == "nested record creation");
		CHECK(ds.getValue("child 2:child 5") == "child 5 record");
		CHECK(ds.getRecord("child 2")->numChildren() == 2);

		{
			EventDelete e("child 1");
			CHECK(e(&ds));
			CHECK(!ds.getRecord("child 1"));
		}

		{
			EventDelete e("child 2:child 3:child 4");
			CHECK(e(&ds))
			CHECK(!ds.getRecord("child 2:child 3:child 4"));
			CHECK(!ds.getRecord("child 2:child 3")); // deletes empty parents
			CHECK(ds.getRecord("child 2"));
			CHECK(ds.getRecord("child 2")->numChildren() == 1);

			CHECK(ds.getValue("child 2:child 5") == "child 5 record"); // non-empty sibling still exists
		}

		{
			EventDelete e("child 2");
			printf("deleting\n");
			CHECK(e(&ds));
			CHECK(!ds.getRecord("child 2")); // children are deleted as well
		}
	};

	TEST("Set") {
		Datastore ds;

		{
			EventSet e("", "test data"); // invalid key
			CHECK(!e(&ds));
			CHECK(ds.getValue("") != "test data");
		}

		{
			EventSet e("child 1", "some key");
			CHECK(e(&ds));
			CHECK(ds.getValue("child 1") == "some key");
		}

		{
			EventSet e("child 1", "some new key"); // overwrite previous value
			CHECK(e(&ds));
			CHECK(ds.getValue("child 1") == "some new key");
		}

		{
			EventSet e("child 1:child 2", "some other key"); // child 1 is a string record, can't have children
			CHECK(!e(&ds));
			CHECK(ds.getValue("child 1:child 2") != "some other key");
		}

		{
			EventSet e("child 2:child 3:child 4", "nested record creation");
			CHECK(e(&ds));
			CHECK(ds.getValue("child 2:child 3:child 4") == "nested record creation");
		}

		{
			EventSet e("child 2:child 5", "child 5 record");
			CHECK(e(&ds));
			CHECK(ds.getValue("child 2:child 5") == "child 5 record");
		}

		CHECK(ds.getValue("child 1") == "some new key");
		CHECK(ds.getValue("child 2:child 3:child 4") == "nested record creation");
		CHECK(ds.getValue("child 2:child 5") == "child 5 record");
	};
};