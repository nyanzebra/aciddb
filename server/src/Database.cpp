#include "Database.h"

Database::Database(int externalPort, const std::string& dsFilename, const std::string& jFilename)
	: _dsFile(dsFilename, std::ios_base::in)
	, _jFile(jFilename)
	, _dataController(_dsFile, _jFile)
{}

void Database::start() {

}

Result Database::processTransaction(const Transaction& transaction) {
	return {};
}

void Database::stop() {

}