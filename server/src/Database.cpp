#ifndef SERVER_PCH_H
#include "pch.h"
#endif
#include "Database.h"

Database::Database(int externalPort, const std::string& dsFilename, const std::string& jFilename)
	: _dsFilename(dsFilename)
	, _jFilename(jFilename)
	, _dsFile(dsFilename, std::ios_base::in)
	, _jFile(jFilename)
	, _dataController(_dsFile, _jFile)
	, _acceptor(_tcpService, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), externalPort))
{}

// TODO: use boost filesystem
#include <cstdio>

Database::~Database() {
	stopAndWait();

	_jFile.close();
	_dsFile.close();

	// TODO: this really belongs to datacontroller
	// TODO: make this operation work properly

	std::ofstream ofs(_dsFilename + ".temp");
	_dataController.saveDB(ofs);
	_dataController.close();
	ofs.close();
	ofs.clear();

	ofs.open(_dsFilename, std::ofstream::out | std::ofstream::trunc);
	ofs.close();
	ofs.clear();

	rename((_dsFilename + ".temp").c_str(), _dsFilename.c_str());

	// erase journal
	ofs.open(_jFilename, std::ofstream::out | std::ofstream::trunc);
	ofs.close();
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
		_transactionNotify.notify_all();
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

void Database::addClient(std::shared_ptr<ClientConnection> client) {
	std::lock_guard<std::mutex> l(_clientsMutex);

	_clients.push_back(client);
}

void Database::removeClient(std::shared_ptr<ClientConnection> client) {
	std::lock_guard<std::mutex> l(_clientsMutex);

	auto it = std::find(_clients.begin(), _clients.end(), client);

	if (it != _clients.end()) {
		Logf(kLogLevelDebug, "client disconnect from %s", (*it)->socket().remote_endpoint().address().to_string().c_str());

		_clients.erase(it);
	}
}

void Database::_killClients() {
	Logf(kLogLevelDebug, "killing clients");
	{
		std::lock_guard<std::mutex> l(_clientsMutex);

		for (auto&& c : _clients) {
			c->stop();
		}
	}
	while (true) {
		std::this_thread::sleep_for(50_ms);
		std::lock_guard<std::mutex> l(_clientsMutex);
		if (_clients.empty()) {
			break;
		}
	}
}

void Database::_processTransactionsThreadEntry() {

	Logf(kLogLevelDebug, "transaction processing thread started");

	TransactionQueue toProcess;

	while (_continueRunning) {
		{
			std::unique_lock<std::mutex> lock(_transactionsMutex);
			auto status = _transactionNotify.wait_for(lock, 1_s);

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

	Logf(kLogLevelDebug, "transaction processing thread ended");

}

void Database::_startAccept() {
	auto client = std::make_shared<ClientConnection>(_tcpService, this);
	addClient(client);
	_acceptor.async_accept(
		client->socket(),
		std::bind(
			&Database::_handleConnect, this,
			client,
			std::placeholders::_1)
	);
}

void Database::_handleConnect(std::shared_ptr<ClientConnection> client, const boost::system::error_code& error) {
	if (!client) { return; }
	Logf(kLogLevelDebug, "new client connected from %s", client->socket().remote_endpoint().address().to_string().c_str());
	if (!error) {
		client->start();
	}
	_startAccept();
}

void Database::_listenThreadEntry() {

	_startAccept();

	_tcpService.run();

	Logf(kLogLevelDebug, "client threads ended");
}