#include <testing.h>

#include <thread>
#include <iostream>
#include <boost/asio.hpp>

#include "../src/Transaction.h"

TEST_SUITE("Transaction") {

	TEST("Send/Receive") {

		try {
			boost::asio::io_service service;
			boost::asio::local::stream_protocol::socket sendSock(service);
			boost::asio::local::stream_protocol::socket recvSock(service);
			boost::asio::local::connect_pair(sendSock, recvSock);

			std::vector<std::string> sent = {"hello", "world", "foo", "bar"};

			SendStrings(sendSock, sent);
			auto received = ReceiveStrings(recvSock);

			CHECK(sent == received);

		} catch (std::exception& e) {
			std::cout << e.what() << std::endl;
			CHECK(false);
		}
	};
};