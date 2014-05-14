#pragma once

#include <thread>
#include <boost/asio.hpp>

#include "../../shared/src/Transaction.h"

#include "ConnectionHandler.h"

class DBContext {
public:	
	DBContext(std::string address, int port);
	DBContext();
	~DBContext();

	bool connect(std::string address, int port);

	Result send(const Transaction& transaction);

private:	

	bool _good = true;

	boost::asio::io_service _service;

	ConnectionHandler _connectionHandler;

	std::thread _serviceThread;

	void _threadEntry();
};