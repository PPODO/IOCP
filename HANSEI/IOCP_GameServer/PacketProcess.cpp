#include "PacketProcess.h"
#include <algorithm>

PacketProcess::PacketProcess(IOCP* Server) : m_Server(Server) {
	m_Database = new DataBaseConnector;

	m_Functions.push_back(std::bind(&PacketProcess::JoinNewPlayer, this, std::placeholders::_1, std::placeholders::_2));
	m_Functions.push_back(std::bind(&PacketProcess::DisconnectFromSession, this, std::placeholders::_1, std::placeholders::_2));
}

PacketProcess::~PacketProcess() {
	if (m_Database) {
		delete m_Database;
		m_Database = nullptr;
	}
}

void PacketProcess::JoinNewPlayer(SOCKETINFO* Info, std::stringstream& RecvStream) {
	PLAYER::Character NewPlayer;
	std::stringstream SendStream;
	std::string SessionName, NickName;

	RecvStream >> SessionName >> NickName;

	auto Data = m_GameInformation.find(SessionName);
	if (Data != m_GameInformation.cend()) {

		SendStream << PM_JOIN << std::endl << EJS_SUCCEDD << std::endl;
		std::cout << "Join New Player Succeed!! : " << SessionName << '\t' << NickName << std::endl;
	}
	else {
		if (m_Database && m_Database->CompareSessionName(SessionName)) {
			PLAYER::CharacterInformation Information;
			NewPlayer.m_PlayerName = NickName;
			NewPlayer.m_Socket = Info->m_Socket;
			Information.m_Characters.push_back(NewPlayer);

			SendStream << PM_JOIN << std::endl << EJS_SUCCEDD << std::endl << Information << std::endl;
			m_GameInformation.insert(std::make_pair(SessionName, Information));
			std::cout << "Join New Player Succeed!! : " << SessionName << '\t' << NickName << std::endl;
		}
		else{
			std::cout << "Join New Player Failure!! : " << SessionName << '\t' << NickName << std::endl;
			SendStream << PM_JOIN << std::endl << EJS_FAILED << std::endl;
			m_Server->Send(Info, SendStream);
			return;
		}
	}
	BroadCast(SendStream, NewPlayer, m_GameInformation.find(SessionName)->second);
}

void PacketProcess::DisconnectFromSession(SOCKETINFO* Info, std::stringstream& RecvStream) {
	std::string SessionName;
	RecvStream >> SessionName;

	auto Iterator = m_GameInformation.find(SessionName);
	if (Iterator != m_GameInformation.cend()) {
		auto It = std::find_if(Iterator->second.m_Characters.cbegin(), Iterator->second.m_Characters.cend(), [&](const PLAYER::Character& Char) -> bool { if (Char.m_Socket == Info->m_Socket) { return true; } return false; });
		if (It != Iterator->second.m_Characters.cend()) {
			Iterator->second.m_Characters.erase(It);
		}
		if (Iterator->second.m_Characters.size() == 0) {
			m_Database->RemoveSession(SessionName);
		}
		else {
			m_Database->RemovePlayerFromSession(SessionName);
		}
	}

	if (Info) {
		closesocket(Info->m_Socket);
		delete Info;
	}
}

void PacketProcess::BroadCast(std::stringstream& SendStream, const PLAYER::Character& NewPlayer, const PLAYER::CharacterInformation& Session) {
	SOCKETINFO* Information = new SOCKETINFO;
	memset(Information, 0, sizeof(SOCKETINFO));

	for (auto It : Session.m_Characters) {
		Information->m_Socket = It.m_Socket;
		m_Server->Send(Information, SendStream);
	}
}