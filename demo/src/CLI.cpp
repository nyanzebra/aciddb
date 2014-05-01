#include "CLI.h"

#include <unistd.h>
#include <stdio.h>

// source site:
// http://stackoverflow.com/questions/717572/how-do-you-do-non-blocking-console-i-o-on-linux-in-c
bool CLI::_InputAvailable() {
	struct timeval tv;
	fd_set fds;
	tv.tv_sec = 0;
	tv.tv_usec = 0;
	FD_ZERO(&fds);
	FD_SET(STDIN_FILENO, &fds);
	select(STDIN_FILENO + 1, &fds, nullptr, nullptr, &tv);
	return (FD_ISSET(0, &fds));
}

void CLI::awaitInput() {
	std::cout << "> ";
	std::cout.flush();
}

void CLI::processInput() {
	std::string line;
	std::getline(std::cin, line);

	for (auto&& e : _observers) {
		e->newInput(line);
	}
}

void CLI::welcome() {
	std::cout << "AcidDB CLI v" ACIDDB_CLI_VERSION << std::endl;
}

void CLI::connect(const std::string& address) {
	std::cout << "connecting to " << address << std::endl;
}