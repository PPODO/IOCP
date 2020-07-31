#pragma once
#include "packet.h"
#include "packet_serializer.h"

void PacketProcessing(const char* buffer) {
	IBitStream istream(buffer);
	FPacketBase basePacket;
	istream >> basePacket;

	switch (basePacket.mPacketType) {
	case EPacketType::Login:

		break;
	case EPacketType::SignUp:

		break;
	}
}