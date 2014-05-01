#include <testing.h>
#include <sstream>
#include <iostream>

#include "../src/Record.h"

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

TEST_SUITE("Record") {

	TEST("Construction") {
		Record r1;

		Record r2("val");

		CHECK(r2 == "val");

		Record r3 = std::move(r2);

		CHECK(r2.getType() == RecordType::kUndefined);

		CHECK(r3 == "val");
	};

	TEST("Usage") {
		Record root;

		root["child 1"].push_back("e0");
		root["child 1"].push_back("e1");
		root["child 1"].push_back("e2");
		root["child 1"].push_back("e3");

		CHECK(root["child 1"].getType() == RecordType::kArray);
		CHECK(root["child 1"].numChildren() == 4);

		CHECK(root["child 1"]["0"].getType() == RecordType::kString);
		CHECK(root["child 1"]["1"].getType() == RecordType::kString);
		CHECK(root["child 1"]["2"].getType() == RecordType::kString);
		CHECK(root["child 1"]["3"].getType() == RecordType::kString);

		CHECK(root["child 1"]["0"] == "e0");
		CHECK(root["child 1"]["1"] == "e1");
		CHECK(root["child 1"]["2"] == "e2");
		CHECK(root["child 1"]["3"] == "e3");

		CHECK(root["child 1"]["0"].getVal() == "e0");
		CHECK(root["child 1"]["1"].getVal() == "e1");
		CHECK(root["child 1"]["2"].getVal() == "e2");
		CHECK(root["child 1"]["3"].getVal() == "e3");

		root["child 2"] = "string";

		CHECK(root["child 2"].getType() == RecordType::kString);
		CHECK(root["child 2"] == "string");
		CHECK(root["child 2"].getVal() == "string");

		CHECK(root["child 3"].getType() == RecordType::kUndefined);
		root["child 3"]["key 1"] = "key 1";
		root["child 3"]["key 2"] = "key 2";

		CHECK(root["child 3"].getType() == RecordType::kAssocArray);
		CHECK(root["child 3"]["key 1"].getType() == RecordType::kString);
		CHECK(root["child 3"]["key 2"].getType() == RecordType::kString);

		root["child 3"]["child 4"]["child 5"] = "key 1";

		CHECK(root.getType() == RecordType::kAssocArray);
		CHECK(root["child 3"].getType() == RecordType::kAssocArray);
		CHECK(root["child 3"]["child 4"].getType() == RecordType::kAssocArray);
		CHECK(root["child 3"]["child 4"]["child 5"].getType() == RecordType::kString);

		root["child 3"].removeChild("child 4");

		CHECK(root["child 3"].getType() == RecordType::kAssocArray);
		CHECK(root["child 3"]["child 4"].getType() == RecordType::kUndefined);

		root.removeChild("child 3");
		CHECK(root.numChildren() == 2);

		root.removeChild("child 2");
		CHECK(root.numChildren() == 1);

		root.removeChild("child 1");
		CHECK(root.numChildren() == 0);
		CHECK(root.getType() == RecordType::kAssocArray);

		root["child"] = "string";
		CHECK(root.numChildren() == 1);

		root.removeAllChildren();
		CHECK(root.numChildren() == 0);
	};

	TEST("Serialization") {

		std::stringstream file;

		{
			boost::archive::text_oarchive oarch(file);

			Record root;

			root["child 1"].push_back("e0");
			root["child 1"].push_back("e1");
			root["child 1"].push_back("e2");
			root["child 1"].push_back("e3");

			root["child 2"] = "string";

			root["child 3"]["key 1"] = "key 1";
			root["child 3"]["key 2"] = "key 2";
			root["child 3"]["child 4"]["child 5"] = "key 1";

			oarch << root;
		}

		boost::archive::text_iarchive iarch(file);

		{
			Record root;
			iarch >> root;

			CHECK(root["child 1"].getType() == RecordType::kArray);
			CHECK(root["child 1"].numChildren() == 4);

			CHECK(root["child 1"]["0"].getType() == RecordType::kString);
			CHECK(root["child 1"]["1"].getType() == RecordType::kString);
			CHECK(root["child 1"]["2"].getType() == RecordType::kString);
			CHECK(root["child 1"]["3"].getType() == RecordType::kString);

			CHECK(root["child 1"]["0"] == "e0");
			CHECK(root["child 1"]["1"] == "e1");
			CHECK(root["child 1"]["2"] == "e2");
			CHECK(root["child 1"]["3"] == "e3");

			CHECK(root["child 1"]["0"].getVal() == "e0");
			CHECK(root["child 1"]["1"].getVal() == "e1");
			CHECK(root["child 1"]["2"].getVal() == "e2");
			CHECK(root["child 1"]["3"].getVal() == "e3");

			CHECK(root["child 2"].getType() == RecordType::kString);
			CHECK(root["child 2"] == "string");
			CHECK(root["child 2"].getVal() == "string");

			CHECK(root["child 3"].getType() == RecordType::kAssocArray);
			CHECK(root["child 3"]["key 1"].getType() == RecordType::kString);
			CHECK(root["child 3"]["key 2"].getType() == RecordType::kString);

			CHECK(root.getType() == RecordType::kAssocArray);
			CHECK(root["child 3"].getType() == RecordType::kAssocArray);
			CHECK(root["child 3"]["child 4"].getType() == RecordType::kAssocArray);
			CHECK(root["child 3"]["child 4"]["child 5"].getType() == RecordType::kString);
		}
	};
};