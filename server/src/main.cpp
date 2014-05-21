#ifndef SERVER_PCH_H
#include "pch.h"
#endif

#include "Database.h"

#define SERVER_VERSION "0.0.1"

void Handler(const boost::system::error_code& error, int signal_number) {
	Logf(kLogLevelInfo, "shutting down");
}

bool ParseCLIArgs(int argc, char* argv[]
	, int* listenPort
	, std::string* dsFilename
	, std::string* jFilename
) try {
	namespace po = boost::program_options;

	po::options_description desc("Allowed options");
	desc.add_options()
		("help", "produce help message")
		("journal", po::value<std::string>(jFilename)->default_value("journal.db"), "set journal file")
		("datastore", po::value<std::string>(dsFilename)->default_value("datastore.db"), "set datastore file")
		("port", po::value<int>(listenPort)->default_value(5999), "set listen port")
	;

	po::variables_map vm;
	po::store(boost::program_options::parse_command_line(argc, argv, desc), vm);

	if (vm.count("help")) {
		std::cout << desc << std::endl;
		return false;
	}

	po::notify(vm);
	return true;

} catch(std::exception& e) {
	Logf("error parsing cli args: %s", e.what());
	return false;
} catch(...) {
	Logf("error parsing cli args");
	return false;
}

int main(int argc, char* argv[]) {

	boost::asio::io_service io_service;
	boost::asio::signal_set signals(io_service, SIGINT, SIGTERM);
	signals.async_wait(Handler);

	int listenPort = 0;
	std::string dsFilename;
	std::string jFilename;

	if (!ParseCLIArgs(argc, argv, &listenPort, &dsFilename, &jFilename)) {
		return 1;
	}
	if (listenPort <= 0) {
		Logf("invalid port");
		return 1;
	}
	if (dsFilename.empty() || jFilename.empty()) {
		Logf("invalid filename");
		return 1;
	}

	Logf("starting aciddb server, v" SERVER_VERSION);

	try {
		Database db(listenPort, dsFilename, jFilename);

		db.start();

		io_service.run();

		Logf(kLogLevelInfo, "stopping database service");

		db.stopAndWait();
	} catch (std::exception& e) {
		Logf(kLogLevelError, "unable to initialize database: %s", e.what());
	}

	return 0;
}