#ifndef SERVER_PCH_H
#include "../src/pch.h"
#endif

#include "../src/Journal.h"
#include "../src/RecordEvent.h"
#include "../src/Datastore.h"

TEST_SUITE("Journal") {

	TEST("Usage") {

		std::stringstream file;

		{
			Journal j(&file);

			RecordEvent e(RecordEventType::kSet, "record", "key");

			j.writeTransaction({e});
		}

		{
			Datastore ds;

			Journal j(&file);

			CHECK(j.applyToDatastore(&ds));

			CHECK(ds.getValue("record") == "key");
		}
	};

};