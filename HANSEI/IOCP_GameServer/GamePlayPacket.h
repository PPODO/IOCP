#pragma once
#include <Windows.h>
#include <sstream>

enum class EPACKETMESSAGETYPE : unsigned char {
	EPMT_JOIN,
	EPMT_UPDATE,
	EPMT_DISCONNECT,
	EPMT_NEWPLAYER,
	EPMT_DISCONNECTOTHER,
	EPMT_COUNT
};

enum class EPACKETFAILEDTYPE : unsigned char {
	EPFT_FAILED,
	EPFT_SUCCEED,
	EPFT_EXIST,
	EPFT_COUNT
};

struct VECTOR {
	float x, y, z;

public:
	VECTOR() : x(0.f), y(0.f), z(0.f) {};

};

struct GAMEPACKET {
	EPACKETMESSAGETYPE m_MessageType;
	EPACKETFAILEDTYPE m_FailedReason;
	int m_SessionID;
	int m_UniqueKey;
	VECTOR m_Location;
	VECTOR m_Rotation;
	UINT_PTR m_Socket;

public:
	GAMEPACKET() : m_MessageType(EPACKETMESSAGETYPE::EPMT_COUNT), m_FailedReason(EPACKETFAILEDTYPE::EPFT_COUNT) {};

public:
	void operator=(const GAMEPACKET& Data) {
		this->m_UniqueKey = Data.m_UniqueKey;
		this->m_Location = Data.m_Location;
		this->m_Rotation = Data.m_Rotation;
		this->m_Socket = Data.m_Socket;
	}

};

class Session {
public:
	int m_SessionID;
	std::string m_SesisonName;
	int m_MaxPlayer;
	bool m_bUsePassword;
	std::string m_Password;
	int m_CurrentPlayer;
	bool m_GameState;

public:
	Session() : m_SessionID(-1), m_SesisonName(""), m_MaxPlayer(0), m_bUsePassword(false), m_Password(""), m_CurrentPlayer(0), m_GameState(false) {};

public:
	friend std::ostream& operator<<(std::ostream& os, Session& Info) {
		os << Info.m_SessionID << std::endl;
		os << Info.m_SesisonName << std::endl;
		os << Info.m_MaxPlayer << std::endl;
		os << Info.m_bUsePassword << std::endl;
		os << Info.m_Password << std::endl;
		os << Info.m_CurrentPlayer << std::endl;
		os << Info.m_GameState << std::endl;

		return os;
	}

	friend std::istream& operator>>(std::istream& is, Session& Info) {
		is >> Info.m_SessionID;
		is >> Info.m_SesisonName;
		is >> Info.m_MaxPlayer;
		is >> Info.m_bUsePassword;
		is >> Info.m_Password;
		is >> Info.m_CurrentPlayer;
		is >> Info.m_GameState;

		return is;
	}
};