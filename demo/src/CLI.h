#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <memory>

#include "version.h"

class CLI {
public:
	class Observer {
	public:
		virtual void newInput(std::string line) = 0;
	};

	inline void writeLine(std::string data) const {	std::cout << data << std::endl; }
	inline void subscribe(std::shared_ptr<Observer> obs) { _observers.push_back(obs); }
	inline bool hasInput() const { return _InputAvailable(); }
	
	void awaitInput();
	void processInput();
	void welcome();
	void connect(const std::string& address);

private:
	std::vector<std::shared_ptr<Observer>> _observers;

	static bool _InputAvailable();
};