#pragma once
#include "DataBase.h"
#include "TimerClass.h"
#include <functional>
#include <vector>
#include <queue>
#include <mutex>
#include <map>

const float ItemRespawnTime = 5.f;

typedef std::function<void(SOCKETINFO*, struct GAMEPACKET*&)> Processor;

class PacketProcessor {
	enum { ETP_LASTTIME, ETP_DELAY, ETP_SESSIONID, ETP_PACKET };
private:
	TimerClass* m_Timer;
	DataBase* m_DataBase;
	class IOCP* m_Server;

private:
	std::vector<Processor> m_Processor;
	std::map<int, struct SessionInformation> m_Sessions;

private:
	std::mutex m_GetSessionInformationLock;
	std::mutex m_GetSessionLock;

public:
	PacketProcessor(class IOCP* Server);
	~PacketProcessor();

private:
	void JoinGame(SOCKETINFO* Info, struct GAMEPACKET*& Packet);
	void UpdatePlayerInformation(SOCKETINFO* Info, struct GAMEPACKET*& Packet);
	void DisconnectPlayer(SOCKETINFO* Info, struct GAMEPACKET*& Packet);
	void PossessingVehicle(SOCKETINFO* Info, struct GAMEPACKET*& Packet);
	void CheckStartFlags(SOCKETINFO* Info, struct GAMEPACKET*& Packet);
	void ChangeReadyState(SOCKETINFO* Info, struct GAMEPACKET*& Packet);
	void PickupItemTypeAndSpawnNewItem(SOCKETINFO* Info, struct GAMEPACKET*& Packet);

private:
	void BroadCast(PACKET* Packet, const std::vector<struct GAMEPACKET>& PlayerList);
	void BroadCast(const std::vector<struct GAMEPACKET>& PlayerList, struct GAMEPACKET*& Packet);

private:
	int UpdatePlayerRanking(std::vector<struct GAMEPACKET>& CharacterLists, std::vector<struct GAMEPACKET>::iterator& Player);
	void ResetPlayerRanking(std::vector<struct GAMEPACKET>& CharacterList, const GAMEPACKET* Packet);

private:
	inline void SwapRanking(int& A, int& B) {
		int NewRank = A;
		A = B;
		B = NewRank;
	}
	void ItemSpawnTimer(const int SessionID, const ITEM ItemInformation);

public:
	Processor& operator[](const EPACKETMESSAGETYPE& MessageType) {
		return m_Processor[static_cast<size_t>(MessageType)];
	}

};