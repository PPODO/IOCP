#define _CRT_SECURE_NO_WARNINGS
#include "DataBaseConnector.h"
#include <iostream>

DataBaseConnector::DataBaseConnector() {
	mysql_init(&m_LoginConnecter);
	mysql_init(&m_SessionConnecter);
	try {
		m_LoginHandler = mysql_real_connect(&m_LoginConnecter, "127.0.0.1", "root", "a2233212", "user_information", 3306, (char*)nullptr, 0);
		m_SessionHandler = mysql_real_connect(&m_SessionConnecter, "127.0.0.1", "root", "a2233212", "session_information", 3306, (char*)nullptr, 0);
		if (!m_LoginHandler) { throw "MySQL Connect Failure!!\n"; }
		if (!m_SessionHandler) { throw "MySQL Connect Failure!!\n"; }
	}
	catch (std::exception& Exception) {
		std::cout << Exception.what() << std::endl;
	}
}

DataBaseConnector::~DataBaseConnector() {
	mysql_close(m_SessionHandler);
	mysql_close(m_LoginHandler);
}

EFAILED DataBaseConnector::AddPlayerToSession(std::string& SessionName, int CurrentPlayer) {
	char Query[MaxQueryLen] = { "\0" };

	sprintf(Query, "UPDATE `information` SET current_player = '%d' WHERE session_name = '%s'", CurrentPlayer, SessionName.c_str());

	if (mysql_query(m_SessionHandler, Query) == 0) {
		return EF_SUCCEED;
	}
	return EF_FAILED;
}

bool DataBaseConnector::CheckIfUserExists(std::string& ID, std::string& Password) {
	char Query[MaxQueryLen] = { "\0" };
	sprintf(Query, "SELECT * FROM `information` WHERE id = '%s' AND password = '%s'", ID.c_str(), Password.c_str());

	if (mysql_query(m_LoginHandler, Query) == 0) {
		MYSQL_RES* Result = mysql_store_result(m_LoginHandler);
		if (Result) {
			MYSQL_ROW Rows = nullptr;
			while ((Rows = mysql_fetch_row(Result))) {
				mysql_free_result(Result);
				return true;
			}
		}
		mysql_free_result(Result);
	}
	return false;
}

bool DataBaseConnector::GetNickNameByID(std::string& ID, std::string& NickName) {
	char Query[MaxQueryLen] = { "\0" };
	sprintf(Query, "SELECT * FROM `information` WHERE id = '%s'", ID.c_str());

	if (mysql_query(m_LoginHandler, Query) == 0) {
		MYSQL_RES* Result = mysql_store_result(m_LoginHandler);
		if (Result) {
			MYSQL_ROW Rows = nullptr;
			while ((Rows = mysql_fetch_row(Result))) {
				if (!Rows[0]) {
					mysql_free_result(Result);
					return false;
				}
				NickName = Rows[0];
				mysql_free_result(Result);
				return true;
			}
		}
		mysql_free_result(Result);
	}
	return false;
}

EFAILED DataBaseConnector::InsertNewAccount(std::string& NickName, std::string& ID, std::string& Password) {
	char Query[MaxQueryLen] = { "\0" };
	sprintf(Query, "SELECT * FROM `information` WHERE nickname = '%s'", NickName.c_str());

	if (mysql_query(m_LoginHandler, Query) == 0) {
		MYSQL_RES* Result = mysql_store_result(m_LoginHandler);
		if (Result) {
			MYSQL_ROW Rows = nullptr;
			while ((Rows = mysql_fetch_row(Result))) {
				mysql_free_result(Result);
				return EF_EXIST;
			}
		}
		mysql_free_result(Result);

		memset(&Query, 0, MaxQueryLen);
		sprintf(Query, "INSERT INTO `information` (`nickname`, `id`, `password`) VALUES ('%s', '%s', '%s')", NickName.c_str(), ID.c_str(), Password.c_str());

		if (mysql_query(m_LoginHandler, Query) == 0) {
			return EF_SUCCEED;
		}
		else {
			return EF_FAILED;
		}
	}
	return EF_FAILED;
}

EFAILED DataBaseConnector::InsertNewSession(const Session& _Session) {
	char Query[MaxQueryLen] = { "\0" };
	sprintf(Query, "SELECT * FROM `information` WHERE session_name = '%s'", _Session.m_SessionName.c_str());

	if (mysql_query(m_SessionHandler, Query) == 0) {
		MYSQL_RES* Result = mysql_store_result(m_SessionHandler);
		if (Result) {
			MYSQL_ROW Rows = nullptr;
			while ((Rows = mysql_fetch_row(Result))) {
				mysql_free_result(Result);
				return EF_EXIST;
			}
		}
		mysql_free_result(Result);

		memset(Query, 0, MaxQueryLen);
		sprintf(Query, "INSERT INTO `information` (`session_name`, `max_player`, `use_password`, `session_password`) VALUES ('%s', '%d', '%d', '%s')", _Session.m_SessionName.c_str(), _Session.m_MaxPlayer, _Session.m_bUsePassword, _Session.m_Password.c_str());

		if (mysql_query(m_SessionHandler, Query) == 0) {
			return EF_SUCCEED;
		}
	}
	return EF_FAILED;
}

int DataBaseConnector::GetAllSessionInformation(SessionInformation& Sessions) {
	int SessionCount = -1;
	char Query[MaxQueryLen];
	sprintf(Query, "SELECT * FROM `information`");

	if (mysql_query(m_SessionHandler, Query) == 0) {
		MYSQL_RES* Result = mysql_store_result(m_SessionHandler);
		if (Result) {
			SessionCount = Result->row_count;
			MYSQL_ROW Rows = nullptr;
			while ((Rows = mysql_fetch_row(Result))) {
				std::stringstream Stream;
				for (int i = 0; i < Result->field_count; i++) {
					Stream << Rows[i] << std::endl;
				}
				Session NewSession;
				Stream >> NewSession;
				Sessions.m_Sessions.push_back(NewSession);
			}
			mysql_free_result(Result);
			return SessionCount;
		}
		mysql_free_result(Result);
	}
	return SessionCount;
}

EJOINFAILED DataBaseConnector::JoinSession(std::string& SessionName, bool UsePassword, std::string& Password) {
	char Query[MaxQueryLen] = { "\0" };

	sprintf(Query, "SELECT * FROM `information` WHERE session_name = '%s'", SessionName.c_str());

	if (mysql_query(m_SessionHandler, Query) == 0) {
		MYSQL_RES* Result = mysql_store_result(m_SessionHandler);
		if (Result) {
			MYSQL_ROW Row = nullptr;
			while ((Row = mysql_fetch_row(Result))) {
				std::stringstream Stream;
				for (int i = 0; i < Result->field_count; i++) {
					Stream << Row[i] << std::endl;
				}
				Session Info;
				Stream >> Info;

				if (Info.m_CurrentPlayer >= Info.m_MaxPlayer) {
					mysql_free_result(Result);
					return EJF_MAXPLAYER;
				}
				if (UsePassword && Info.m_Password != Password) {
					mysql_free_result(Result);
					return EJF_WRONGPASS;
				}
				mysql_free_result(Result);
				return AddPlayerToSession(SessionName, Info.m_CurrentPlayer + 1) == EF_SUCCEED ? EJF_SUCCEED : EJF_FAILED;
			}
		}
		mysql_free_result(Result);
		return EJF_INVALIDSESSION;
	}
	return EJF_INVALIDSESSION;
}