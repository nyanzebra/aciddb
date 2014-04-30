#include <stdio.h>
#include <unistd.h>
#include <thread>
#include <chrono>

#include "CLI.h"
#include "Interpreter.h"

static bool gShouldContinue = true;

void sig_handler(int signo) {
	gShouldContinue = false;
}

int main (int argc, char const *argv[]) {

	auto cli = std::make_shared<CLI>();

	auto interpreter = std::make_shared<Interpreter>(cli);

	cli->subscribe(interpreter);

	cli->header();

	cli->awaitInput();

	while (gShouldContinue) {
		if (cli->hasInput()) {
			cli->processInput();
			cli->awaitInput();
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
	}
	
	return 0;
}