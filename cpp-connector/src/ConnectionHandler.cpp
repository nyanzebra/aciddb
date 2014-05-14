#include <boost/array.hpp>

#include "../../shared/src/logging.h"
#include "../../shared/src/packets.h"

#include "ConnectionHandler.h"

ConnectionHandler::ConnectionHandler(boost::asio::io_service& service)
	: _socket(service) 
	, _resolver(service)
{
}

bool ConnectionHandler::connect(std::string address, int port)
try {
	boost::asio::ip::tcp::resolver::query query(address, std::to_string(port));
	boost::asio::ip::tcp::resolver::iterator endpoint_iterator = _resolver.resolve(query);

	boost::system::error_code error;

	boost::asio::connect(_socket, endpoint_iterator);

	Logf(kLogLevelDebug, "connection established with %s, port %d", address.c_str(), port);

	return true;
} catch (std::exception &e) {
	Logf(kLogLevelDebug, e.what());
	return false;
}

void ConnectionHandler::sendTransaction(const Transaction& transaction) {
	SendStrings(_socket, transaction);
}

Result ConnectionHandler::receiveResult() {
	return ReceiveStrings(_socket);
}