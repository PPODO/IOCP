#pragma once
#include "IOCP_ForMain.h"
#include "DataBaseConnector.h"
#include <sstream>
#include <vector>
#include <functional>

static const int MaxNickNameLen = 15;
static const int MaxIDLen = 15;

enum PACKETMESSAGE {
	PM_SIGNUP,
	PM_LOGIN,
	PM_CREATESESSION,
	PM_SESSIONLIST,
	PM_JOINSESSION,
	PM_COUNT
};

class PacketProcess {
private:
	class IOCP_ForMain* m_Server;
	DataBaseConnector* m_DB;

private:
	std::vector<std::function<void(SOCKETINFO*, std::stringstream&)>> m_Functions;

private:
	void Login(SOCKETINFO* Info, std::stringstream& RecvStream);
	void SignUp(SOCKETINFO* Info, std::stringstream& RecvStream);
	void CreateSession(SOCKETINFO* Info, std::stringstream& RecvStream);
	void GetAllSessionInformation(SOCKETINFO* Info, std::stringstream& RecvStream);
	void JoinSession(SOCKETINFO* Info, std::stringstream& RecvStream);

public:
	PacketProcess(IOCP_ForMain* IOCP);
	~PacketProcess();

public:
	std::function<void(SOCKETINFO*, std::stringstream&)>& operator[](const PACKETMESSAGE& PM) {
		if (PM < m_Functions.size()) {
			return m_Functions[PM];
		}
	}
};