#pragma once
#include <Windows.h>
#include <sstream>

static const int NickNameMaxLen = 15;

enum class EPACKETMESSAGETYPE : unsigned char {
	EPMT_JOIN,
	EPMT_UPDATE,
	EPMT_DISCONNECT,
	EPMT_STARTGAME,
	EPMT_SPAWNITEM,
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

enum class EPACKETTYPE : unsigned char {
	EPT_PLAYER,
	EPT_SPAWNER,
	EPT_COUNT
};

struct VECTOR {
	float x, y, z;

public:
	VECTOR() : x(0.f), y(0.f), z(0.f) {};

};

struct FInputMotionData {
public:
	float m_Steering;
	float m_Throttle;
	bool m_HandBreak;

	FInputMotionData() : m_Steering(0.f), m_Throttle(0.f), m_HandBreak(false) {};
	FInputMotionData(float& Steering, float Throttle, bool HandBreak) : m_Steering(Steering), m_Throttle(Throttle), m_HandBreak(HandBreak) {};
};

struct PACKET {
	EPACKETTYPE m_PacketType;
	EPACKETMESSAGETYPE m_MessageType;
	EPACKETFAILEDTYPE m_FailedReason;

public:
	PACKET() : m_PacketType(EPACKETTYPE::EPT_COUNT), m_MessageType(EPACKETMESSAGETYPE::EPMT_COUNT), m_FailedReason(EPACKETFAILEDTYPE::EPFT_COUNT) {};

};

struct GAMEPACKET : public PACKET {
	int m_SessionID;
	int m_UniqueKey;
	VECTOR m_Location;
	VECTOR m_Rotation;
	FInputMotionData m_VehicleData;
	UINT_PTR m_Socket;
	char m_PlayerNickName[NickNameMaxLen];

public:
	void operator=(const GAMEPACKET& Data) {
		this->m_UniqueKey = Data.m_UniqueKey;
		this->m_Location = Data.m_Location;
		this->m_Rotation = Data.m_Rotation;
		this->m_Socket = Data.m_Socket;
		this->m_VehicleData = Data.m_VehicleData;
	}

};

struct SPAWNERPACKET : public PACKET {


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