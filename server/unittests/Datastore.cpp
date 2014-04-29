#include <testing.h>
#include <iostream>
#include <sstream>
#include <boost/archive/text_oarchive.hpp>

#include "../src/Datastore.h"
#include "../src/Record.h"

TEST_SUITE("Datastore") {

	TEST("Usage") {

		std::stringstream infile;

		// build test file
		{
			boost::archive::text_oarchive oarch(infile);
		
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

		Datastore ds1(infile);

		CHECK(ds1.good());

		CHECK(!strcmp(ds1.getValue("child 1:0"), "e0"));
		CHECK(!strcmp(ds1.getValue("child 1:1"), "e1"));
		CHECK(!strcmp(ds1.getValue("child 1:2"), "e2"));
		CHECK(!strcmp(ds1.getValue("child 1:3"), "e3"));
		
		CHECK(!strcmp(ds1.getValue("child :1"), "")); // doesn't exist
		CHECK(!strcmp(ds1.getValue("child :2"), "")); // doesn't exist

		CHECK(!strcmp(ds1.getValue("child 2"), "string"));
		CHECK(!strcmp(ds1.getValue("child 3:key 1"), "key 1"));
		CHECK(!strcmp(ds1.getValue("child 3:key 2"), "key 2"));
		CHECK(!strcmp(ds1.getValue("child 3:child 4:child 5"), "key 1"));

		std::stringstream outfile;

		ds1.write(outfile);

		Datastore ds2(outfile);

		CHECK(ds2.good());

		CHECK(!strcmp(ds2.getValue("child 1:0"), "e0"));
		CHECK(!strcmp(ds2.getValue("child 1:1"), "e1"));
		CHECK(!strcmp(ds2.getValue("child 1:2"), "e2"));
		CHECK(!strcmp(ds2.getValue("child 1:3"), "e3"));
		
		CHECK(!strcmp(ds2.getValue("child :1"), "")); // doesn't exist
		CHECK(!strcmp(ds2.getValue("child :2"), "")); // doesn't exist

		CHECK(!strcmp(ds2.getValue("child 2"), "string"));
		CHECK(!strcmp(ds2.getValue("child 3:key 1"), "key 1"));
		CHECK(!strcmp(ds2.getValue("child 3:key 2"), "key 2"));
		CHECK(!strcmp(ds2.getValue("child 3:child 4:child 5"), "key 1"));
	};

};