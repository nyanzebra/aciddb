#include "Database.h"

#include "../../shared/src/common.h"
#include "../../shared/src/logging.h"

Database::Database(int externalPort, const std::string& dsFilename, const std::string& jFilename)
	: _dsFile(dsFilename, std::ios_base::in)
	, _jFile(jFilename)
	, _dataController(_dsFile, _jFile)
{}

Database::~Database() {
	stopAndWait();
}

void Database::start() {
	_transactionThread = std::thread(std::bind(&Database::_processTransactionsThreadEntry, this));
	_networkThread = std::thread(std::bind(&Database::_listenThreadEntry, this));
}

Result Database::receivedTransaction(const Transaction& transaction) {

	std::condition_variable cv;
	std::mutex m;
	Result ret;

	std::unique_lock<std::mutex> resultLock(m);

	{
		std::unique_lock<std::mutex> txnLock(_transactionsMutex);

		_transactionQueue.emplace_back(
			transaction,
			[&](Result result) {
				std::unique_lock<std::mutex> lock(m);
				ret = std::move(result);
				lock.unlock();
				cv.notify_one();
			}
		);
	}

	while (_continueRunning) {
		auto status = cv.wait_for(resultLock, 1_s);
		if (status != std::cv_status::timeout) {
			break;
		}
	}

	return ret;
}

void Database::stop() {
	_continueRunning = false;
}

void Database::stopAndWait() {
	_continueRunning = false;

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
			txn.second(std::move(result));
		}
		toProcess.clear();
	}

}

void Database::_listenThreadEntry() {
	while (_continueRunning) {
		std::this_thread::sleep_for(1_s);
		// TODO: receive network connections
	}

	Logf(kLogLevelDebug, "killing client threads");

	for (auto&& c : _clients) {
		c->stop();
	}
	for (auto&& c : _clients) {
		c->stopAndWait();
	}
	Logf(kLogLevelDebug, "client threads disconnected");
}