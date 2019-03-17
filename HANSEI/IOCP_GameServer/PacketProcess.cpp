#include "PacketProcess.h"
#include <algorithm>

PacketProcess::PacketProcess(IOCP* Server) : m_Server(Server) {
	m_Database = new DataBaseConnector;

	m_Functions.push_back(std::bind(&PacketProcess::JoinNewPlayer, this, std::placeholders::_1, std::placeholders::_2));
	m_Functions.push_back(std::bind(&PacketProcess::DisconnectFromSession, this, std::placeholders::_1, std::placeholders::_2));
	m_Functions.push_back(std::bind(&PacketProcess::UpdatePlayerInformation, this, std::placeholders::_1, std::placeholders::_2));
}

PacketProcess::~PacketProcess() {
	if (m_Database) {
		delete m_Database;
		m_Database = nullptr;
	}
}

void PacketProcess::JoinNewPlayer(SOCKETINFO* Info, std::stringstream& RecvStream) {
	std::stringstream SendStream;
	unsigned int UniqueKey = 0;
	std::string SessionName, NickName;
	RecvStream >> SessionName >> NickName >> UniqueKey;

	auto SessionData = m_GameInformation.find(SessionName);
	if (SessionData != m_GameInformation.cend()) {
		if (m_Database && m_Database->AddPlayerToSession(SessionName) != EF_FAILED) {
			PLAYER::Character NewPlayer;
			NewPlayer.m_UniqueKey = UniqueKey;
			NewPlayer.m_PlayerName = NickName;
			NewPlayer.m_Socket = Info->m_Socket;

			// 기존의 플레이어들에게 새로운 플레이어가 들어왔음을 알리는 메시지를 보냄.
			std::cout << "Join Session Succeed!! : " << SessionName << '\t' << NickName << std::endl;
			SendStream << PM_NEWPLAYER << std::endl << EJS_SUCCEDD << std::endl << 1 << std::endl << NewPlayer;
			BroadCast(SendStream, SessionData->second);
			SessionData->second.m_Characters.push_back(NewPlayer);
			
			SendStream.str("");
			SendStream << PM_JOIN << std::endl << EJS_SUCCEDD << std::endl << SessionData->second.m_Characters.size() << std::endl << SessionData->second;
			m_Server->Send(Info, SendStream);
			return;
		}
	}
	else {
		if (m_Database && m_Database->CompareSessionName(SessionName) && m_Database->AddPlayerToSession(SessionName) != EF_FAILED) {
			PLAYER::Character NewPlayer;
			PLAYER::CharacterInformation Information;
			NewPlayer.m_UniqueKey = UniqueKey;
			NewPlayer.m_PlayerName = NickName;
			NewPlayer.m_Socket = Info->m_Socket;
			Information.m_Characters.push_back(NewPlayer);

			std::cout << "Join Session Succeed!! : " << SessionName << '\t' << NickName << std::endl;
			m_GameInformation.insert(std::make_pair(SessionName, Information));
			SendStream << PM_JOIN << std::endl << EJS_SUCCEDD << std::endl << Information.m_Characters.size() << std::endl << Information;
			m_Server->Send(Info, SendStream);
			return;
		}
	}
	std::cout << "Join Session Failure!! : " << SessionName << '\t' << NickName << std::endl;
	SendStream << PM_JOIN << std::endl << EJS_FAILED << std::endl;
	m_Server->Send(Info, SendStream);
}

void PacketProcess::DisconnectFromSession(SOCKETINFO* Info, std::stringstream& RecvStream) {
	std::stringstream SendStream;
	std::string SessionName;
	RecvStream >> SessionName; 

	auto Iterator = m_GameInformation.find(SessionName);
	if (Iterator != m_GameInformation.cend()) {
		auto It = std::find_if(Iterator->second.m_Characters.cbegin(), Iterator->second.m_Characters.cend(), [&](const PLAYER::Character& Char) -> bool { if (Char.m_Socket == Info->m_Socket) { return true; } return false; });
		if (It != Iterator->second.m_Characters.cend()) {
			unsigned int Key = It->m_UniqueKey;
			Iterator->second.m_Characters.erase(It);

			SendStream << PM_DISCONNECTOTHER << std::endl << Key << std::endl;
			BroadCast(SendStream, Iterator->second);
		}
		// No one left in Session
		if (Iterator->second.m_Characters.size() == 0) {
			m_GameInformation.erase(Iterator);
			m_Database->RemoveSession(SessionName);
		}
		else {
			m_Database->RemovePlayerFromSession(SessionName);
		}
	}
}

void PacketProcess::UpdatePlayerInformation(SOCKETINFO* Info, std::stringstream& RecvStream) {
	std::string SessionName;
	PLAYER::Character CharacterInformation;
	std::stringstream SendStream;

	RecvStream >> SessionName >> CharacterInformation;
	
	auto Session = m_GameInformation.find(SessionName);
	if (Session != m_GameInformation.cend()) {
		auto Player = std::find_if(Session->second.m_Characters.begin(), Session->second.m_Characters.end(), [&CharacterInformation](const PLAYER::Character& Information) -> bool { if (Information.m_UniqueKey == CharacterInformation.m_UniqueKey) { return true; } return false; });
		if (Player != Session->second.m_Characters.cend()) {
			(*Player) = CharacterInformation;
			SendStream << PM_UPDATE << std::endl << Player->m_UniqueKey << std::endl << (*Player);
			BroadCast(SendStream, Session->second, Player->m_UniqueKey);
		}
	}
}

void PacketProcess::BroadCast(std::stringstream& SendStream, const PLAYER::CharacterInformation& Session) {
	SOCKETINFO* Info = new SOCKETINFO;
	memset(Info, 0, sizeof(SOCKETINFO));

	for (auto It : Session.m_Characters) {
		Info->m_Socket = It.m_Socket;
		m_Server->Send(Info, SendStream);
	}
}

void PacketProcess::BroadCast(std::stringstream & SendStream, const PLAYER::CharacterInformation & Session, unsigned int Key) {
	SOCKETINFO* Info = new SOCKETINFO;
	memset(Info, 0, sizeof(SOCKETINFO));

	for (auto It : Session.m_Characters) {
		if (Key != It.m_UniqueKey) {
			Info->m_Socket = It.m_Socket;
			m_Server->Send(Info, SendStream);
		}
	}
}