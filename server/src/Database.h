#pragma once

#include <fstream>

#include "DataController.h"

class Database {
public:
	Database(int externalPort, const std::string& dsFilename, const std::string& jFilename);

	void start();

	Result processTransaction(const Transaction& transaction);

	void stop();

private:
	std::fstream _dsFile;
	std::fstream _jFile;
	DataController _dataController;
};