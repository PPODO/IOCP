#include "IOCP.h"
#include "PacketProcessor.h"
#include <iostream>

PacketProcessor::PacketProcessor(IOCP* Server) : m_Server(Server), m_DataBase(nullptr) {
	m_DataBase = new DataBase;
	if (!m_DataBase || !m_DataBase->ClearAllSessions()) {
		throw "Init DataBase Failure!";
	}

	m_Processor.push_back(std::bind(&PacketProcessor::Login, this, std::placeholders::_1, std::placeholders::_2));
	m_Processor.push_back(std::bind(&PacketProcessor::NewAccount, this, std::placeholders::_1, std::placeholders::_2));
	m_Processor.push_back(std::bind(&PacketProcessor::CreateSession, this, std::placeholders::_1, std::placeholders::_2));
	m_Processor.push_back(std::bind(&PacketProcessor::JoinSession, this, std::placeholders::_1, std::placeholders::_2));
	m_Processor.push_back(std::bind(&PacketProcessor::GetSessions, this, std::placeholders::_1, std::placeholders::_2));
}

PacketProcessor::~PacketProcessor() {
	m_Server = nullptr;

	if (m_DataBase) {
		delete m_DataBase;
		m_DataBase = nullptr;
	}
}

void PacketProcessor::Login(SOCKETINFO* Info, std::stringstream& RecvStream) {
	std::stringstream SendStream;
	std::string ID, Password, UserName;

	RecvStream >> ID >> Password;
	if (Info && m_DataBase && m_Server) {
		if (m_DataBase->TryLogin(ID, Password, UserName)) {
			std::cout << "Server : Login Succeed! - " << Info->m_Socket << '\t' << UserName << std::endl;
			SendStream << EPMT_LOGIN << std::endl << EPFT_SUCCEED << std::endl << UserName << std::endl;
			m_Server->Send(Info, SendStream);
			return;
		}
		std::cout << "Server : Login Failure! - " << Info->m_Socket << std::endl;
		SendStream << EPMT_LOGIN << std::endl << EPFT_FAILED << std::endl;
		m_Server->Send(Info, SendStream);
	}
}

void PacketProcessor::NewAccount(SOCKETINFO* Info, std::stringstream& RecvStream) {
	std::stringstream SendStream;
	std::string ID, Password, UserName;

	RecvStream >> UserName >> ID >> Password;

	if (Info && m_DataBase && m_Server) {
		if (m_DataBase->IsItExistUserInfo(ID, Password, UserName) && m_DataBase->InsertNewAccount(UserName, ID, Password)) {
			std::cout << "Server : Create Account Succeed! - " << Info->m_Socket << '\t' << UserName << std::endl;
			SendStream << EPMT_NEWACCOUNT << std::endl << EPFT_SUCCEED << std::endl << UserName << std::endl;
			m_Server->Send(Info, SendStream);
			return;
		}
		else {
			std::cout << "Server : Create Account Failure! - " << Info->m_Socket << std::endl;
			SendStream << EPMT_NEWACCOUNT << std::endl << EPFT_EXIST << std::endl;
			m_Server->Send(Info, SendStream);
			return;
		}
		std::cout << "Server : Create Account Failure! - " << Info->m_Socket << std::endl;
		SendStream << EPMT_NEWACCOUNT << std::endl << EPFT_FAILED << std::endl;
		m_Server->Send(Info, SendStream);
	}
}

void PacketProcessor::CreateSession(SOCKETINFO* Info, std::stringstream& RecvStream) {
	std::stringstream SendStream;
	std::string SessionName, Password;
	int MaxPlayer, SessionID = -1;
	bool UsePassword;
	RecvStream >> SessionName >> MaxPlayer >> UsePassword >> Password;

	if (Info && m_DataBase && m_Server) {
		if (m_DataBase->IsItExistSessionInfo(SessionName) && m_DataBase->InsertNewSession(SessionID, SessionName, MaxPlayer, UsePassword, Password)) {
			std::cout << "Server : Create Session Succeed! - " << Info->m_Socket << '\t' << SessionName << std::endl;
			SendStream << EPMT_CREATESESSION << std::endl << EPFT_SUCCEED << std::endl << SessionID << std::endl;
			m_Server->Send(Info, SendStream);
			return;
		}
		else {
			std::cout << "Server : Create Session Failure! - " << Info->m_Socket << std::endl;
			SendStream << EPMT_CREATESESSION << std::endl << EPFT_EXIST << std::endl;
			m_Server->Send(Info, SendStream);
			return;
		}
		std::cout << "Server : Create Session Failure! - " << Info->m_Socket << std::endl;
		SendStream << EPMT_CREATESESSION << std::endl << EPFT_FAILED << std::endl;
		m_Server->Send(Info, SendStream);
	}
}

void PacketProcessor::JoinSession(SOCKETINFO* Info, std::stringstream& RecvStream) {
	std::stringstream SendStream;
	std::string SessionName, Password;
	bool bUsePassword;
	int SessionID = -1;

	RecvStream >> SessionName >> bUsePassword >> Password;

	if (Info && m_DataBase && m_Server && SessionName.length() > 0) {
		if (m_DataBase->TryJoinSession(SessionID, SessionName, bUsePassword, Password)) {
			std::cout << "Join Session Is Succeed!! - " << Info->m_Socket << '\t' << SessionName << std::endl;
			SendStream << EPMT_JOINSESSION << std::endl << EPFT_SUCCEED << std::endl << SessionID << std::endl;
			m_Server->Send(Info, SendStream);
			return;
		}
		SendStream << EPMT_JOINSESSION << std::endl << EPFT_FAILED << std::endl;
		m_Server->Send(Info, SendStream);
	}
}

void PacketProcessor::GetSessions(SOCKETINFO* Info, std::stringstream& RecvStream) {
	std::stringstream SendStream;
	SessionInformation Information;
	size_t SessionCount = 0, CurrentPage = 0;
	RecvStream >> CurrentPage;

	if (Info && m_DataBase && m_Server) {
		if (m_DataBase->GetSessionInformation(Information, CurrentPage, SessionCount)) {
			std::cout << "Find Sesison is Succeed! - " << Info->m_Socket << std::endl;
			SendStream << EPMT_GETSESSION << std::endl << EPFT_SUCCEED << std::endl << SessionCount << std::endl << Information.m_Sessions.size() << std::endl << Information << std::endl;

			m_Server->Send(Info, SendStream);
			return;
		}
		std::cout << "Find Sesison is Failure! - " << Info->m_Socket << std::endl;
	}
}