#include <functional>
#include <chrono>
#include <arpa/inet.h>

#include "../../shared/src/common.h"
#include "../../shared/src/logging.h"
#include "../../shared/src/packets.h"
#include "ClientConnection.h"
#include "Database.h"

ClientConnection::ClientConnection(boost::asio::io_service& service, Database* db)
	: _db(db)
	, _service(service)
	, _socket(service)
{
	// TODO: keepalive?
}

ClientConnection::ClientConnection(ClientConnection&& source)
	: _db(source._db)
	, _service(source._service)
	, _socket(std::move(source._socket))
{
	source._db = nullptr;
}

ClientConnection::~ClientConnection() {
	_closeSocket();
	if (_connectionThread.joinable()) {
		_connectionThread.join();
	}
}

void ClientConnection::_threadEntry() {
	try {
		while (_continueRunning) {
			auto txn = _getTransaction();

			auto result = _db->processTransaction(txn);

			_sendResult(result);

		}
	} catch(std::exception &e) {
		if (_continueRunning) {
			Logf(kLogLevelDebug, e.what());
		}
	}

	// this thread needs to finish before the client can destruct. Kill the client from the service thread.
	_service.post([=]() { _db->removeClient(shared_from_this()); });
}

Transaction ClientConnection::_getTransaction() {
	return ReceiveStrings(_socket);
}

void ClientConnection::_sendResult(Result result) {
	SendStrings(_socket, result);
}

void ClientConnection::start() {
	if (!_db) { return; }
	_connectionThread = std::thread(std::bind(&ClientConnection::_threadEntry, this));
}

void ClientConnection::stop() {
	_continueRunning = false;
	_closeSocket();
}

void ClientConnection::_closeSocket() {
	try {
		_socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both);
	} catch (...) {}
	try {
		_socket.close();
	} catch (...) {}
}