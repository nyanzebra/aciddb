#pragma once

class Database;

class ClientConnection : public std::enable_shared_from_this<ClientConnection> {
public:
	ClientConnection(boost::asio::io_service& service, Database* db);
	ClientConnection(ClientConnection&& source);
	~ClientConnection();

	void start();
	void stop(); // async

	boost::asio::ip::tcp::socket& socket() { return _socket; }

private:
	Database* _db;
	boost::asio::io_service& _service;
	boost::asio::ip::tcp::socket _socket;

	std::thread _connectionThread;

	bool _continueRunning = true;

	void _closeSocket();

	void _threadEntry();

	void _handlewrite(const boost::system::error_code& error, size_t bytesTransferred);

	Transaction _getTransaction();
	void _sendResult(Result result);
};