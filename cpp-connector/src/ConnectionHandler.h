#pragma once

#include <boost/asio.hpp>
#include <vector>
#include <memory>

#include "../../shared/src/Transaction.h"

class ConnectionHandler {
public:
	ConnectionHandler(boost::asio::io_service& service);

	bool connect(std::string address, int port);

	void sendTransaction(const Transaction& transaction);
	Result receiveResult();

	void sendReceiveLoop();
private:
	std::function<void(const boost::system::error_code&, size_t)> _receiver;

	boost::asio::ip::tcp::socket _socket;
	boost::asio::ip::tcp::resolver _resolver;
};