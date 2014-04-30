#include "ClientConnection.h"

#include <functional>
#include <chrono>

#include "../../shared/src/common.h"

ClientConnection::ClientConnection()
	: _clientThread(std::bind(&ClientConnection::threadEntry, this))
{}

ClientConnection::~ClientConnection() {
	stopAndWait();
}

void ClientConnection::threadEntry() {

	while (_continueRunning) {
		std::this_thread::sleep_for(1_s);
		
		Transaction txn;

		// TODO: get actual transactions
		txn = {"set x y", "get x"};

		if (!txn.empty()) {
			for (auto&& obs : _observers) {
				auto result = obs->receivedTransaction(txn);
				// TODO: send result back
			}
		}
	}

}

void ClientConnection::stopAndWait() {
	_continueRunning = false;

	if (_clientThread.joinable()) {
		_clientThread.join();
	}
}

void ClientConnection::stop() {
	_continueRunning = false;
}

void ClientConnection::subscribe(Observer* observer) {
	_observers.push_back(observer);
}