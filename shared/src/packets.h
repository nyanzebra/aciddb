#pragma once

enum PacketType : uint32_t {
	kPacketTypeTransaction = 0x1,
	kPacketTypeResult      = 0x2,
};

struct PacketHeader {
	PacketType type;
	uint32_t numStatements;
};