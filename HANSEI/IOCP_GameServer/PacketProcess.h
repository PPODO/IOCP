#pragma once
#include "IOCP.h"
#include "DataBaseConnector.h"
#include "Networking.h"
#include <functional>
#include <vector>
#include <tuple>
#include <map>

enum PACKETMESSAGE {
	PM_JOIN,
	PM_DISCONNECT,
	PM_COUNT
};

enum EJOINSTATE {
	EJS_FAILED,
	EJS_SUCCEDD
};

class PacketProcess {
private:
	IOCP* m_Server;
	DataBaseConnector* m_Database;

private:
	std::map<std::string, PLAYER::CharacterInformation> m_GameInformation;

private:
	std::vector<std::function<void(SOCKETINFO*, std::stringstream&)>> m_Functions;

public:
	PacketProcess(IOCP* Server);
	~PacketProcess();

private:
	void BroadCast(std::stringstream& SendStream, const PLAYER::Character& NewPlayer, const PLAYER::CharacterInformation& Session);

private:
	void JoinNewPlayer(SOCKETINFO* Info, std::stringstream& RecvStream);
	void DisconnectFromSession(SOCKETINFO* Info, std::stringstream& RecvStream);

public:
	std::function<void(SOCKETINFO*, std::stringstream&)>& operator[](const PACKETMESSAGE& PacketType) {
		if (PacketType < m_Functions.size()) {
			return m_Functions[PacketType];
		}
	}

};