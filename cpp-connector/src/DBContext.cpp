#include <boost/serialization/vector.hpp>
#include <sstream>
#include <arpa/inet.h>

#include "../../shared/src/logging.h"

#include "DBContext.h"

DBContext::DBContext(std::string address, int port)
	: DBContext()
{
	_good = _connectionHandler.connect(std::move(address), port);
}

DBContext::DBContext()
	: _connectionHandler(_service)
{}

DBContext::~DBContext() {
	_service.stop();
	if (_serviceThread.joinable()) {
		_serviceThread.join();	
	}
}

void DBContext::_threadEntry() {
	boost::asio::io_service::work work(_service);
	_service.run();
	Logf(kLogLevelDebug, "DBConnection service thread terminated");
}

bool DBContext::connect(std::string address, int port) {
	if (_connectionHandler.connect(std::move(address), port)) {
		_serviceThread = std::thread(std::bind(&DBContext::_threadEntry, this));
		return true;
	}
	return false;
}

Result DBContext::send(const Transaction& transaction) {

	_connectionHandler.sendTransaction(transaction);

	return _connectionHandler.receiveResult();
}