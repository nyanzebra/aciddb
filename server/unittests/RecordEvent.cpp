#include <testing.h>

#include <sstream>

#include "../src/config.h"
#include "../src/RecordEvent.h"
#include "../src/Datastore.h"

TEST_SUITE("RecordEvents") {

	TEST("Usage") {
		Datastore ds;
		RecordEvent(RecordEventType::kSet, "child", "key")(&ds);

		CHECK(ds.getRoot()->numChildren() == 1);
	};

	TEST("Move") {

		Datastore ds;

		RecordEvent(RecordEventType::kSet, "child 1", "some new key")(&ds);
		RecordEvent(RecordEventType::kSet, "child 2:child 3:child 4", "nested record creation")(&ds);
		RecordEvent(RecordEventType::kSet, "child 2:child 5", "child 5 record")(&ds);

		{
			RecordEvent e(RecordEventType::kMove, "", "to"); // invalid keys
			CHECK(!e(&ds));
		}

		// data should still exist
		CHECK(ds.getValue("child 1") == "some new key");
		CHECK(ds.getValue("child 2:child 3:child 4") == "nested record creation");
		CHECK(ds.getValue("child 2:child 5") == "child 5 record");
		CHECK(ds.getRecord("child 2")->numChildren() == 2);

		{
			RecordEvent e(RecordEventType::kMove, "child 1", "child 1:subchild"); // nesting further
			CHECK(e(&ds));
			CHECK(ds.getRecord("child 1"));
			CHECK(ds.getRecord("child 1:subchild"));
			CHECK(ds.getValue("child 1:subchild") == "some new key");
		}

		{
			RecordEvent e(RecordEventType::kMove, "child 1:subchild", "child 1"); // de-nesting
			CHECK(e(&ds));
			CHECK(ds.getRecord("child 1"));
			CHECK(!ds.getRecord("child 1:subchild"));
			CHECK(ds.getValue("child 1") == "some new key");
		}

		{
			RecordEvent e(RecordEventType::kMove, "child 2", "new child 2");
			CHECK(e(&ds));
			CHECK(!ds.getRecord("child 2"));
			CHECK(ds.getRecord("new child 2"));
			CHECK(ds.getValue("new child 2:child 3:child 4") == "nested record creation");
			CHECK(ds.getValue("new child 2:child 5") == "child 5 record");
		}

		{
			RecordEvent e(RecordEventType::kMove, "new child 2:child 3:child 4", "child 2:new child 4");
			CHECK(e(&ds));
			CHECK(!ds.getRecord("child 2:child 3:child 4"));
			CHECK(!ds.getRecord("child 2:child 3")); // deletes empty parents
			CHECK(ds.getRecord("child 2"));
			CHECK(ds.getRecord("child 2:new child 4"));
			CHECK(ds.getRecord("child 2")->numChildren() == 1);
		}
	};

	TEST("Delete") {

		Datastore ds;

		RecordEvent(RecordEventType::kSet, "child 1", "some new key")(&ds);
		RecordEvent(RecordEventType::kSet, "child 2:child 3:child 4", "nested record creation")(&ds);
		RecordEvent(RecordEventType::kSet, "child 2:child 5", "child 5 record")(&ds);

		{
			RecordEvent e(RecordEventType::kDelete, ""); // invalid key
			CHECK(e(&ds));
		}

		// data should still exist
		CHECK(ds.getValue("child 1") == "some new key");
		CHECK(ds.getValue("child 2:child 3:child 4") == "nested record creation");
		CHECK(ds.getValue("child 2:child 5") == "child 5 record");
		CHECK(ds.getRecord("child 2")->numChildren() == 2);

		{
			RecordEvent e(RecordEventType::kDelete, "child 1");
			CHECK(e(&ds));
			CHECK(!ds.getRecord("child 1"));
		}

		{
			RecordEvent e(RecordEventType::kDelete, "child 2:child 3:child 4");
			CHECK(e(&ds))
			CHECK(!ds.getRecord("child 2:child 3:child 4"));
			CHECK(!ds.getRecord("child 2:child 3")); // deletes empty parents
			CHECK(ds.getRecord("child 2"));
			CHECK(ds.getRecord("child 2")->numChildren() == 1);

			CHECK(ds.getValue("child 2:child 5") == "child 5 record"); // non-empty sibling still exists
		}

		{
			RecordEvent e(RecordEventType::kDelete, "child 2");
			CHECK(e(&ds));
			CHECK(!ds.getRecord("child 2")); // children are deleted as well
		}
	};

	TEST("Set") {
		Datastore ds;

		{
			RecordEvent e(RecordEventType::kSet, "", "test data"); // invalid key
			CHECK(!e(&ds));
			CHECK(ds.getValue("") != "test data");
		}

		{
			RecordEvent e(RecordEventType::kSet, "child 1", "some key");
			CHECK(e(&ds));
			CHECK(ds.getValue("child 1") == "some key");
		}

		{
			RecordEvent e(RecordEventType::kSet, "child 1", "some new key"); // overwrite previous value
			CHECK(e(&ds));
			CHECK(ds.getValue("child 1") == "some new key");
		}

		{
			RecordEvent e(RecordEventType::kSet, "child 1:child 2", "some other key"); // child 1 is a string record, can't have children
			CHECK(!e(&ds));
			CHECK(ds.getValue("child 1:child 2") != "some other key");
		}

		{
			RecordEvent e(RecordEventType::kSet, "child 2:child 3:child 4", "nested record creation");
			CHECK(e(&ds));
			CHECK(ds.getValue("child 2:child 3:child 4") == "nested record creation");
		}

		{
			RecordEvent e(RecordEventType::kSet, "child 2:child 5", "child 5 record");
			CHECK(e(&ds));
			CHECK(ds.getValue("child 2:child 5") == "child 5 record");
		}

		CHECK(ds.getValue("child 1") == "some new key");
		CHECK(ds.getValue("child 2:child 3:child 4") == "nested record creation");
		CHECK(ds.getValue("child 2:child 5") == "child 5 record");
	};

	TEST("Edge Cases") {

		Datastore ds;

		RecordEvent(RecordEventType::kSet, "root:child1", "key1")(&ds);
		RecordEvent(RecordEventType::kSet, "root:child2:temp", "key1")(&ds);

		CHECK(ds.getRecord("root:child1")->getType() == RecordType::kString);

		RecordEvent(RecordEventType::kDelete, "root:child2")(&ds);

		CHECK(ds.getRecord("root:child1"));
		CHECK(!ds.getRecord("root:child2"));

		RecordEvent(RecordEventType::kMove, "root:child1", "root:temp:child1")(&ds);

		CHECK(!ds.getRecord("root:child1"));
		CHECK(ds.getRecord("root:temp:child1"));
		CHECK(ds.getRecord("root:temp:child1")->getType() == RecordType::kString);

		RecordEvent(RecordEventType::kMove, "root:child1", "root:temp:child1")(&ds);

		CHECK(!ds.getRecord("root:child1"));
		CHECK(ds.getRecord("root:temp:child1"));
		CHECK(ds.getRecord("root:temp:child1")->getType() == RecordType::kString);

		RecordEvent(RecordEventType::kDelete, "root:child1")(&ds);

		CHECK(!ds.getRecord("root:child1"));
		CHECK(ds.getRecord("root:temp:child1"));

		CHECK(ds.getRecord("root:temp:child1")->getType() == RecordType::kString);

	};

	TEST("Serialization") {
		Datastore ds;

		std::stringstream file;

		{
			OutputArchiveType oarch(file);

			RecordEvent e1(RecordEventType::kSet, "root:child1", "key1");
			RecordEvent e2(RecordEventType::kSet, "root:child2:temp", "key2");
			
			oarch << e1 << e2;
		}

		{
			InputArchiveType iarch(file);

			std::vector<RecordEvent> events;

			try {
				while (true) {
					RecordEvent e;
					iarch >> e;
					events.push_back(std::move(e));
				}
			} catch (...) {}

			CHECK(events.size() == 2);
			for (auto&& e : events) {
				e(&ds);
			}

			CHECK(ds.getValue("root:child1") == "key1");
			CHECK(ds.getValue("root:child2:temp") == "key2");
		}
	};
};