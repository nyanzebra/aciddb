#include "Database.h"

#include <future>

#include "../../shared/src/common.h"
#include "../../shared/src/logging.h"

Database::Database(int externalPort, const std::string& dsFilename, const std::string& jFilename)
	: _dsFile(dsFilename, std::ios_base::in)
	, _jFile(jFilename)
	, _dataController(_dsFile, _jFile)
	, _acceptor(_tcpService, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), externalPort))
{}

Database::~Database() {
	stopAndWait();
}

void Database::start() {
	_transactionThread = std::thread(std::bind(&Database::_processTransactionsThreadEntry, this));
	_networkThread = std::thread(std::bind(&Database::_listenThreadEntry, this));
}

Result Database::processTransaction(const Transaction& transaction) {

	std::promise<Result> p;
	auto futureResult = p.get_future();

	{
		std::unique_lock<std::mutex> txnLock(_transactionsMutex);
		_transactionQueue.emplace_back(transaction, std::move(p));
	}

	while (_continueRunning) {
		auto status = futureResult.wait_for(1_s);
		if (status == std::future_status::ready) {
			break;
		}
	}

	return futureResult.get();
}

void Database::stop() {
	_continueRunning = false;
}

void Database::stopAndWait() {
	_continueRunning = false;
	_tcpService.stop();

	if (_networkThread.joinable()) {
		_networkThread.join();
	}
	if (_transactionThread.joinable()) {
		_transactionThread.join();
	}
}

void Database::_processTransactionsThreadEntry() {

	Logf(kLogLevelDebug, "transaction processing thread started");

	TransactionQueue toProcess;

	while (_continueRunning) {
		{
			std::unique_lock<std::mutex> l(_transactionsMutex);
			auto status = _transactionNotify.wait_for(l, 1_s);

			if (status == std::cv_status::timeout) {
				continue;
			}

			size_t s = toProcess.size();
			toProcess.resize(s + _transactionQueue.size());
			std::move(_transactionQueue.begin(), _transactionQueue.end(), toProcess.begin() + s);
			_transactionQueue.clear();
		}
		
		for (auto&& txn : toProcess) {
			auto result = _dataController.processTransaction(txn.first);
			txn.second.set_value(std::move(result));
		}
		toProcess.clear();
	}

}

void Database::_startAccept() {
	auto client = std::make_shared<ClientConnection>(_tcpService, this);
	_acceptor.async_accept(
		client->socket(),
		std::bind(
			&Database::_handleConnect, this,
			client,
			std::placeholders::_1)
	);
}

void Database::_handleConnect(std::shared_ptr<ClientConnection> client, const boost::system::error_code& error) {
	Logf(kLogLevelDebug, "handle connect");
	if (!error) {
		client->start();
	}
	_startAccept();
}

void Database::_listenThreadEntry() {

	_startAccept();

	_tcpService.run();

	Logf(kLogLevelDebug, "client threads disconnected");
}