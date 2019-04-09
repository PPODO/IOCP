#pragma once
#include "DataBase.h"
#include "GamePlayPacket.h"
#include <functional>
#include <vector>

typedef std::function<void(SOCKETINFO*, GAMEPACKET*&)> Processor;

class PacketProcessor {
private:
	DataBase* m_DataBase;
	class IOCP* m_Server;

private:
	std::vector<Processor> m_Processor;

public:
	PacketProcessor(class IOCP* Server);
	~PacketProcessor();

private:
	void JoinGame(SOCKETINFO* Info, GAMEPACKET*& Packet);

public:
	Processor& operator[](const size_t& Index) {
		return m_Processor[Index];
	}

};