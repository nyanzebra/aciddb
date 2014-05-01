#pragma once

#include <boost/asio.hpp>

#include "../../shared/src/types.h"

class Database;

class ClientConnection {
public:
	ClientConnection(boost::asio::io_service& socket, Database* db);
	ClientConnection(ClientConnection&& source);
	~ClientConnection();

	void start();

	boost::asio::ip::tcp::socket& socket() { return _socket; }

private:
	Database* _db;
	boost::asio::ip::tcp::socket _socket;

	void _closeSocket();
};