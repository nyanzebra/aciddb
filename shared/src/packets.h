#pragma once

enum PacketType : uint32_t {
	kPacketTypePrefixedStrings = 0x1,
};

struct PacketHeader {
	PacketType type;
	uint32_t length;
};