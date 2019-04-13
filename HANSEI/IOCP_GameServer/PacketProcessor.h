#pragma once
#include "DataBase.h"
#include <functional>
#include <vector>
#include <mutex>
#include <map>

typedef std::function<void(SOCKETINFO*, struct GAMEPACKET*&)> Processor;

class PacketProcessor {
private:
	std::mutex m_Lock;
	DataBase* m_DataBase;
	class IOCP* m_Server;

private:
	std::vector<Processor> m_Processor;
	std::map<int, std::vector<struct GAMEPACKET>> m_Sessions;

public:
	PacketProcessor(class IOCP* Server);
	~PacketProcessor();

private:
	void JoinGame(SOCKETINFO* Info, struct GAMEPACKET*& Packet);
	void UpdatePlayerInformation(SOCKETINFO* Info, struct GAMEPACKET*& Packet);
	void DisconnectPlayer(SOCKETINFO* Info, struct GAMEPACKET*& Packet);

private:
	void BroadCast(struct GAMEPACKET*& Packet, const std::vector<struct GAMEPACKET>& PlayerList);
	void BroadCast(const std::vector<struct GAMEPACKET>& PlayerList, struct GAMEPACKET*& Packet);

public:
	Processor& operator[](const size_t& Index) {
		return m_Processor[Index];
	}

};