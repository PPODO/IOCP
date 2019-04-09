#pragma once
#include <string>
#include <vector>

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

class SessionInformation {
public:
	std::vector<Session> m_Sessions;

public:
	friend std::ostream& operator<<(std::ostream& os, SessionInformation& Information) {
		for (auto It : Information.m_Sessions) {
			os << It;
		}

		return os;
	}

	friend std::istream& operator>>(std::istream& is, SessionInformation& Information) {
		size_t Count = 0;
		is >> Count;

		for (size_t i = 0; i < Count; i++) {
			Session NewSession;
			is >> NewSession;
			Information.m_Sessions.push_back(NewSession);
		}
		return is;
	}
};