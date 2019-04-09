#pragma once
#include "DataBase.h"
#include "SessionInformation.h"
#include <vector>
#include <sstream>
#include <functional>

typedef std::function<void(SOCKETINFO*, std::stringstream&)> Processor;

enum EPACKETMESSAGETYPE {
	EPMT_LOGIN,
	EPMT_NEWACCOUNT,
	EPMT_CREATESESSION,
	EPMT_JOINSESSION,
	EPMT_GETSESSION,
	EPMT_COUNT
};

enum EPACKETFAILEDTYPE {
	EPFT_FAILED,
	EPFT_SUCCEED,
	EPFT_EXIST
};

class PacketProcessor {
private:
	DataBase* m_DataBase;
	IOCP_Base* m_Server;

private:
	std::vector<Processor> m_Processor;

private:
	void Login(SOCKETINFO* Info, std::stringstream& RecvStream);
	void NewAccount(SOCKETINFO* Info, std::stringstream& RecvStream);
	void CreateSession(SOCKETINFO* Info, std::stringstream& RecvStream);
	void JoinSession(SOCKETINFO* Info, std::stringstream& RecvStream);
	void GetSessions(SOCKETINFO* Info, std::stringstream& RecvStream);

public:
	PacketProcessor(class IOCP* Server);
	~PacketProcessor();

public:
	inline Processor& operator[] (const size_t& Index) {
		return m_Processor[Index];
	}

};