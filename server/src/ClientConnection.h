#pragma once

#include <memory>
#include <thread>

#include "types.h"

class ClientConnection : public std::enable_shared_from_this<ClientConnection> {
public:
	class Observer {
	public:
		virtual Result receivedTransaction(const Transaction& transaction) = 0;
	};

	ClientConnection();
	~ClientConnection();

	void threadEntry();

	void stopAndWait();
	void stop();

	void subscribe(Observer* observer);

private:
	bool _continueRunning = true;

	std::thread _clientThread;

	std::vector<Observer*> _observers;
};