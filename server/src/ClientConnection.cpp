#include "ClientConnection.h"

#include <functional>
#include <chrono>

#include "../../shared/src/common.h"
#include "../../shared/src/logging.h"
#include "Database.h"

ClientConnection::ClientConnection(boost::asio::io_service& service, Database* db)
	: _db(db)
	, _socket(service)
{}

ClientConnection::ClientConnection(ClientConnection&& source)
	: _db(source._db)
	, _socket(std::move(source._socket))
{
	source._db = nullptr;
}

ClientConnection::~ClientConnection() {
	_closeSocket();
}

void ClientConnection::start()
try {
	if (!_db) { return; }

	std::stringstream buffer;

	while (true) {
		boost::array<char, 128> buf;
		boost::system::error_code error;

		std::cout << "reading" << std::endl;
		size_t len = _socket.read_some(boost::asio::buffer(buf), error);
		std::cout << "read" << std::endl;

		if (error == boost::asio::error::eof) {
			std::cout << "eof" << std::endl;
			break;
		} else if (error) {
			throw boost::system::system_error(error); // Some other error.
		}

		buffer.write(buf.data(), len);

		std::cout.write(buf.data(), len);

		// TODO: check buffer?
	}

	// try {
	// 	OutputArchiveType oarch(ss);

	// 	Transaction txn;
		
	// 	oarch >> txn;

	// 	Result result = _db->processTransaction(txn);

	// 	std::stringstream resultStream;
	// 	InputArchiveType iarch(resultStream);

	// 	iarch << result;

	// 	_socket.send(resultStream);

	// } catch (...) {
	// 	Result r({"server could not parse transaction"})

	// 	std::stringstream resultStream;
	// 	InputArchiveType iarch(resultStream);

	// 	iarch << result;
		
	// 	_socket.write(resultStream);
	// }
} catch(std::exception &e) {
	std::cout << e.what() << std::endl;
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