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
	PacketHeader header;

	// TODO: refactor this

	boost::asio::read(_socket, boost::asio::buffer(&header, sizeof(header)));

	PacketType type = (PacketType)ntohl(header.type);
	uint32_t numStatements = ntohl(header.numStatements);

	if (type != kPacketTypeTransaction) {
		throw std::runtime_error("invalid packet type");
	}

	Transaction txn;

	for (size_t i = 0; i < numStatements; ++i) {
		uint32_t length = 0;
		boost::asio::read(_socket, boost::asio::buffer(&length, sizeof(length)));
		length = ntohl(length);

		// TODO: sanity check this stuff

		std::string statement;
		statement.resize(length);

		boost::asio::read(_socket, boost::asio::buffer(&statement[0], length));
		txn.emplace_back(std::move(statement));
	}

	return txn;
}

void ClientConnection::_sendResult(Result result) {
	std::vector<boost::asio::const_buffer> buffers;

	// TODO: refactor this

	PacketHeader header;
	std::vector<uint32_t> sizes;

	buffers.push_back(boost::asio::buffer(&header, sizeof(header)));

	// construct a series of buffers that, when read sequentially, will produce the correct packet.
	for (auto&& statement : result) {
		sizes.push_back(htonl(statement.size()));
		buffers.emplace_back(boost::asio::buffer(&sizes.back(), sizeof(sizes.back())));
		buffers.emplace_back(boost::asio::buffer(statement));
	}

	auto numStatements = result.size();

	if (numStatements > std::numeric_limits<uint32_t>::max()) {
		throw std::runtime_error("overflow in buffer size");
	}

	header.type = (PacketType)htonl(kPacketTypeResult);
	header.numStatements = htonl((uint32_t)numStatements);

	boost::asio::write(_socket, buffers);
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