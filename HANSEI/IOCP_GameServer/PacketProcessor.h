#pragma once
#include "DataBase.h"
#include <functional>
#include <utility>
#include <vector>
#include <mutex>
#include <map>

typedef std::function<void(SOCKETINFO*, struct PACKET*&)> Processor;

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
	void JoinGame(SOCKETINFO* Info, struct PACKET*& Packet);
	void UpdatePlayerInformation(SOCKETINFO* Info, struct PACKET*& Packet);
	void DisconnectPlayer(SOCKETINFO* Info, struct PACKET*& Packet);
	void StartGame(SOCKETINFO* Info, struct PACKET*& Packet);



private:
	void BroadCast(struct GAMEPACKET*& Packet, const std::vector<struct GAMEPACKET>& PlayerList);
	void BroadCast(const std::vector<struct GAMEPACKET>& PlayerList, struct GAMEPACKET*& Packet);

public:
	Processor& operator[](const EPACKETMESSAGETYPE& MessageType) {
		return m_Processor[static_cast<size_t>(MessageType)];
	}

};