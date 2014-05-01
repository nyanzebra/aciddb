#pragma once

#include <fstream>
#include <vector>
#include <memory>
#include <thread>
#include <mutex>
#include <deque>
#include <condition_variable>
#include <boost/asio.hpp>
#include <future>

#include "DataController.h"
#include "ClientConnection.h"

class Database {
public:
	Database(int externalPort, const std::string& dsFilename, const std::string& jFilename);
	~Database();

	void start();

	void stop();
	void stopAndWait();

	/**
	 * @brief Processes a single transaction and blocks until completion
	 * @details The call to processTransaction may take some time and is designed
	 * to be called by client threads. Transactions are processed sequentially
	 * and so eventually the call will return, but there may be a number of other
	 * transactions that must execute first.
	 * 
	 * @param transaction Transaction to execute
	 * @return Results for the statements in the transaction
	 */
	Result processTransaction(const Transaction& transaction);

private:
	std::fstream _dsFile;
	std::fstream _jFile;
	DataController _dataController;
	
	typedef std::vector<std::pair<Transaction, std::promise<Result>>> TransactionQueue;
	
	TransactionQueue _transactionQueue;
	std::mutex _transactionsMutex;

	bool _continueRunning = true;
	std::thread _transactionThread;
	std::thread _networkThread;
	std::condition_variable _transactionNotify;

	void _processTransactionsThreadEntry();

	boost::asio::io_service _tcpService;
	boost::asio::ip::tcp::acceptor _acceptor;
	void _listenThreadEntry();

	void _startAccept();
	void _handleConnect(std::shared_ptr<ClientConnection> client, const boost::system::error_code& error);
};