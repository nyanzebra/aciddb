#include <stdio.h>
#include <unistd.h>
#include <thread>
#include <chrono>

#include "../../cpp-connector/src/DBContext.h"

#include "CLI.h"
#include "Interpreter.h"

static bool gShouldContinue = true;

void sig_handler(int signo) {
	gShouldContinue = false;
}

int main (int argc, char const *argv[]) {

	auto cli = std::make_shared<CLI>();

	DBContext context;

	auto interpreter = std::make_shared<Interpreter>(cli, &context);

	cli->subscribe(interpreter);
	cli->welcome();

	if (!context.connect("localhost", 5999)) {
		cli->writeLine("unable to connect to localhost");
		return 1;
	}

	cli->connect("localhost");

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