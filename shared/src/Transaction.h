#pragma once

#include <string>
#include <vector>
#include <boost/asio.hpp>

#include "packets.h"

#include <iostream>

typedef std::vector<std::string> Result;
typedef std::vector<std::string> Transaction;

#define STRING_SIZE_MAX_BYTES 1024

/**
 * @brief Writes a container as a set of length-prefixed strings to a socket.
 * @details The container needs to be compatible with the ranged based for loop and
 * the function will also prefix the write call with the total length, in bytes,
 * of the data it sends.
 *
 * @param socket Stream socket to write to
 * @param strings Container with strings
 */
template <typename T, typename ContainerType>
void SendStrings(boost::asio::basic_stream_socket<T>& socket, const ContainerType& strings) {
	std::vector<boost::asio::const_buffer> buffers;

	PacketHeader header;
	std::vector<uint32_t> sizes(strings.size());

	buffers.push_back(boost::asio::buffer(&header, sizeof(header)));

	// construct a series of buffers that, when read sequentially, will produce the correct packet.
	for (auto&& str : strings) {
		uint32_t size = str.size();
		sizes.push_back(htonl(size));
		buffers.emplace_back(boost::asio::buffer(&sizes.back(), sizeof(sizes.back())));
		buffers.emplace_back(boost::asio::buffer(str));
	}

	size_t length = boost::asio::buffer_size(buffers) - sizeof(PacketHeader);

	if (length > std::numeric_limits<uint32_t>::max()) {
		throw std::runtime_error("overflow in buffer size");
	}

	header.type = (PacketType)htonl(kPacketTypePrefixedStrings);
	header.length = htonl((uint32_t)length);

	boost::asio::write(socket, buffers);
}

/**
 * @brief Receives a set of length-prefixed strings over a socket.
 * @details The receive function is set up to receive data presented in the same
 * format that the SendStrings function provides.
 *
 * @param socket Socket to receive from
 * @return vector containing the strings it receives
 */
template <typename T>
std::vector<std::string> ReceiveStrings(boost::asio::basic_stream_socket<T>& socket) {

	PacketHeader header;

	boost::asio::read(socket, boost::asio::buffer(&header, sizeof(header)));

	PacketType type = (PacketType)ntohl(header.type);
	uint32_t remLength = ntohl(header.length);

	if (type != kPacketTypePrefixedStrings) {
		throw std::runtime_error("invalid packet type");
	}

	std::vector<std::string> strings;

	while (remLength > 0) {
		uint32_t length = 0;

		boost::asio::read(socket, boost::asio::buffer(&length, sizeof(length)));
		length = ntohl(length);
		remLength -= sizeof(length);

		if (length > STRING_SIZE_MAX_BYTES) {
			throw std::runtime_error("string to receive is too large");
		}

		std::string str;
		str.resize(length);

		boost::asio::read(socket, boost::asio::buffer(&str[0], length));
		strings.emplace_back(std::move(str));

		remLength -= length;
	}

	return strings;
}