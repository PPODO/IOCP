#include "PacketProcess.h"
#include "Networking.h"
#include <iostream>

PacketProcess::PacketProcess(IOCP_ForMain* IOCP) : m_Server(IOCP) {
	m_DB = new DataBaseConnector;

	m_Functions.push_back(std::bind(&PacketProcess::SignUp, this, std::placeholders::_1, std::placeholders::_2));
	m_Functions.push_back(std::bind(&PacketProcess::Login, this, std::placeholders::_1, std::placeholders::_2));
	m_Functions.push_back(std::bind(&PacketProcess::CreateSession, this, std::placeholders::_1, std::placeholders::_2));
	m_Functions.push_back(std::bind(&PacketProcess::GetAllSessionInformation, this, std::placeholders::_1, std::placeholders::_2));
	m_Functions.push_back(std::bind(&PacketProcess::JoinSession, this, std::placeholders::_1, std::placeholders::_2));
}

PacketProcess::~PacketProcess() {
	if (m_DB) {
		delete m_DB;
		m_DB = nullptr;
	}
}

void PacketProcess::Login(SOCKETINFO* Info, std::stringstream& RecvStream) {
	std::stringstream SendStream;
	std::string ID, PASSWORD;

	RecvStream >> ID >> PASSWORD;

	if (m_DB->CheckIfUserExists(ID, PASSWORD)) {
		std::string NickName;
		if (m_DB->GetNickNameByID(ID, NickName)) {
			std::cout << "Login Successful!! : " << NickName << std::endl;
			SendStream << PM_LOGIN << std::endl << EF_SUCCEED << std::endl << NickName << std::endl;
			m_Server->Send(Info, SendStream);
			return;
		}
	}
	std::cout << "Login Failure!!" << std::endl;
	SendStream << PM_LOGIN << std::endl << EF_EXIST << std::endl;
	m_Server->Send(Info, SendStream);
}

void PacketProcess::SignUp(SOCKETINFO* Info, std::stringstream& RecvStream) {
	std::stringstream SendStream;
	std::string NICKNAME, ID, PASSWORD;

	RecvStream >> NICKNAME >> ID >> PASSWORD;

	EFAILED FailedReason = m_DB->InsertNewAccount(NICKNAME, ID, PASSWORD);
	
	if (!m_DB->CheckIfUserExists(ID, PASSWORD)) {
		if (FailedReason == EF_FAILED) {
			std::cout << "Create Account Failure!!\n";
			SendStream << PM_SIGNUP << std::endl << EF_FAILED << std::endl;
			m_Server->Send(Info, SendStream);
			return;
		}
		std::cout << "Create Account Successful!! : " << NICKNAME << '\t' << ID << '\t' << PASSWORD << std::endl;
	}
	else if(FailedReason == EF_EXIST) {
		std::cout << "Create Account Failure!!\n";
		SendStream << PM_SIGNUP << std::endl << EF_EXIST << std::endl;
		m_Server->Send(Info, SendStream);
		return;
	}
	SendStream << PM_SIGNUP << std::endl << EF_SUCCEED << std::endl;
	m_Server->Send(Info, SendStream);
}

void PacketProcess::CreateSession(SOCKETINFO* Info, std::stringstream& RecvStream) {
	std::stringstream SendStream;

	Session NewSession;
	RecvStream >> NewSession;

	EFAILED FailedReason = m_DB->InsertNewSession(NewSession);
	switch (FailedReason) {
	case EF_FAILED:
		std::cout << "Create Session Failure!!\n";
		SendStream << PM_CREATESESSION << std::endl << EF_FAILED << std::endl;
		return;
	case EF_EXIST:
		std::cout << "Create Session Failure!!\n";
		SendStream << PM_CREATESESSION << std::endl << EF_EXIST << std::endl;
		return;
	case EF_SUCCEED:
		std::cout << "Create Session Successful!! : " << NewSession.m_SessionName << std::endl;
		SendStream << PM_CREATESESSION << std::endl << EF_SUCCEED << std::endl;
		return;
	}
	m_Server->Send(Info, SendStream);
}

void PacketProcess::GetAllSessionInformation(SOCKETINFO* Info, std::stringstream& RecvStream) {
	std::stringstream SendStream;
	int SessionCount;
	SessionInformation SessionInformations;

	if ((SessionCount = m_DB->GetAllSessionInformation(SessionInformations)) > 0) {
		std::cout << "Find Session Successful!! : " << SessionCount << std::endl;
		SendStream << PM_SESSIONLIST << std::endl << EF_SUCCEED << std::endl << SessionCount << std::endl << std::endl << SessionInformations;
		m_Server->Send(Info, SendStream);
		return;
	}
	std::cout << "Find Session Failure!! : " << SessionCount << std::endl;
	SendStream << PM_SESSIONLIST << std::endl << EF_FAILED << std::endl << SessionCount << std::endl << std::endl << SessionInformations;
	m_Server->Send(Info, SendStream);
}

void PacketProcess::JoinSession(SOCKETINFO* Info, std::stringstream& RecvStream) {
	std::stringstream SendStream;
	std::string SessionName, Password;
	bool UsePassword;

	RecvStream >> SessionName >> UsePassword >> Password;

	EJOINFAILED Reason = m_DB->JoinSession(SessionName, UsePassword, Password);
	switch (Reason) {
	case EJF_FAILED:
		std::cout << "Join Session Failure! : " << "FAILED\n";
		SendStream << PM_JOINSESSION << std::endl << EJF_FAILED << std::endl;
		break;
	case EJF_INVALIDSESSION:
		std::cout << "Join Session Failure! : " << "INVALIDSESSION\n";
		SendStream << PM_JOINSESSION << std::endl << EJF_INVALIDSESSION << std::endl;
		break;
	case EJF_MAXPLAYER:
		std::cout << "Join Session Failure! : " << "MAXPLAYER\n";
		SendStream << PM_JOINSESSION << std::endl << EJF_MAXPLAYER << std::endl;
		break;
	case EJF_WRONGPASS:
		std::cout << "Join Session Failure! : " << "WRONGPASSWORD\n";
		SendStream << PM_JOINSESSION << std::endl << EJF_WRONGPASS << std::endl;
		break;
	case EJF_SUCCEED:
		SendStream << PM_JOINSESSION << std::endl << EJF_SUCCEED << std::endl;
		std::cout << "Join Session Succeed!\n";
		break;
	}
	m_Server->Send(Info, SendStream);
}