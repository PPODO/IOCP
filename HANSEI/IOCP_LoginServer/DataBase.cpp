#include "DataBase.h"
#include <iostream>
#include <sstream>

DataBase::DataBase() : m_LoginConnector(nullptr) {
	mysql_init(&m_LoginHandle);
	mysql_init(&m_SessionHandle);

	try {
		m_LoginConnector = mysql_real_connect(&m_LoginHandle, "127.0.0.1", "root", "a2233212", "user_information", 3306, nullptr, 0);
		m_SessionConnector = mysql_real_connect(&m_SessionHandle, "127.0.0.1", "root", "a2233212", "session_information", 3306, nullptr, 0);

		if (!m_LoginConnector) {
			throw "Init Data Base Failure!\n";
		}
		if (!m_SessionConnector) {
			throw "Init Data Base Failure!\n";
		}
	}
	catch (const std::string& Exception) {
		std::cout << Exception << std::endl;
	}
}

DataBase::~DataBase() {
	mysql_close(&m_LoginHandle);
	mysql_close(&m_SessionHandle);
}

bool DataBase::ClearAllSessions() {
	char Query[MaxQuerySize] = { "\0" };
	sprintf(Query, "DELETE FROM `information`");

	if (mysql_query(m_SessionConnector, Query) == 0) {
		return true;
	}
	return false;
}

bool DataBase::TryLogin(const std::string & ID, const std::string & Password, std::string & UserName) {
	char Query[MaxQuerySize] = { "\0" };
	sprintf(Query, "SELECT* FROM `information` WHERE `id` = '%s' and `password` = '%s'", ID.c_str(), Password.c_str());

	if (mysql_query(m_LoginConnector, Query) == 0) {
		MYSQL_RES* Result = mysql_store_result(m_LoginConnector);
		if (Result) {
			MYSQL_ROW Rows = nullptr;
			while ((Rows = mysql_fetch_row(Result))) {
				UserName = Rows[0];
				mysql_free_result(Result);
				return true;
			}
		}
	}
	return false;
}

bool DataBase::IsItExistUserInfo(const std::string & ID, const std::string & Password, const std::string & UserName) {
	char Query[MaxQuerySize] = { "\0" };
	sprintf(Query, "SELECT* FROM `information` WHERE `nickname` = '%s' OR `id` = '%s'", UserName.c_str(), ID.c_str());

	if (mysql_query(m_LoginConnector, Query) == 0) {
		MYSQL_RES* Result = mysql_store_result(m_LoginConnector);
		if (Result && Result->row_count > 0) {
			mysql_free_result(Result);
			return false;
		}
	}
	return true;
}

bool DataBase::IsItExistSessionInfo(const std::string & SessionName) {
	char Query[MaxQuerySize] = { "\0" };
	sprintf(Query, "SELECT* FROM `information` WHERE `session_name` = '%s'", SessionName.c_str());

	if (mysql_query(m_SessionConnector, Query) == 0) {
		MYSQL_RES* Result = mysql_store_result(m_SessionConnector);
		if (Result && Result->row_count > 0) {
			mysql_free_result(Result);
			return false;
		}
	}
	return true;
}

bool DataBase::GetSessionInformation(SessionInformation & Information, const size_t& CurrentPage, size_t& SessionCount) {
	char Query[MaxQuerySize] = { "\0" };
	sprintf(Query, "SELECT * FROM `information` LIMIT %d, 8", CurrentPage);

	if (mysql_query(m_SessionConnector, Query) == 0) {
		MYSQL_RES* Result = mysql_store_result(m_SessionConnector);
		if (Result) {
			MYSQL_ROW Rows = nullptr;
			while ((Rows = mysql_fetch_row(Result))) {
				std::stringstream Stream;
				Session SessionInfo;

				for (size_t i = 0; i < Result->field_count; i++) {
					Stream << Rows[i] << std::endl;
				}
				Stream >> SessionInfo;
				Information.m_Sessions.push_back(SessionInfo);
			}
			mysql_free_result(Result);

			memset(Query, 0, MaxQuerySize);
			sprintf(Query, "SELECT* FROM `information`");

			if (mysql_query(m_SessionConnector, Query) == 0) {
				MYSQL_RES* Result = mysql_store_result(m_SessionConnector);
				if (Result) {
					SessionCount = Result->row_count;
					return true;
				}
			}
		}
	}
	return false;
}

bool DataBase::TryJoinSession(int& SessionID, const std::string & SessionName, const bool & UsePassword, const std::string & Password) {
	char Query[MaxQuerySize] = { "\0" };

	if (UsePassword) {
		sprintf(Query, "SELECT* FROM `information` WHERE `session_name` = '%s' AND `session_password` = '%s'", SessionName.c_str(), Password.c_str());
	}
	else {
		sprintf(Query, "SELECT* FROM `information` WHERE `session_name` = '%s'", SessionName.c_str());
	}

	if (mysql_query(m_SessionConnector, Query) == 0) {
		MYSQL_RES* Result = mysql_store_result(m_SessionConnector);
		if (Result && Result->row_count > 0) {
			MYSQL_ROW Rows = nullptr;
			while ((Rows = mysql_fetch_row(Result))) {
				std::stringstream Stream;
				Session SessionInfo;
				for (size_t i = 0; i < Result->field_count; i++) {
					Stream << Rows[i] << std::endl;
				}
				Stream >> SessionInfo;

				if (SessionInfo.m_CurrentPlayer + 1 <= SessionInfo.m_MaxPlayer) {
					SessionID = SessionInfo.m_SessionID;
					mysql_free_result(Result);
					return true;
				}
				mysql_free_result(Result);
			}
		}
	}
	return false;
}

bool DataBase::InsertNewAccount(const std::string & UserName, const std::string & ID, const std::string & Password) {
	char Query[MaxQuerySize] = { "\0" };
	sprintf(Query, "INSERT INTO `information` (`nickname`, `id`, `password`) values ('%s', '%s', '%s')", UserName.c_str(), ID.c_str(), Password.c_str());

	if (mysql_query(m_LoginConnector, Query) == 0) {
		return true;
	}
	return false;
}

bool DataBase::InsertNewSession(int& SessionID, const std::string & SessionName, const int & MaxPlayer, const bool & bUsePassword, const std::string & Password) {
	char Query[MaxQuerySize] = { "\0" };
	sprintf(Query, "SELECT* FROM `information`");

	if (mysql_query(m_SessionConnector, Query) == 0) {
		MYSQL_RES* Result = mysql_store_result(m_SessionConnector);
		if (Result) {
			SessionID = Result->row_count + 1;
		}
		mysql_free_result(Result);

		if (SessionID >= 0) {
			memset(Query, 0, MaxQuerySize);
			sprintf(Query, "INSERT INTO `information` (`session_id`, `session_name`, `max_player`, `use_password`, `session_password`) values ('%d', '%s', '%d', '%d', '%s')", SessionID, SessionName.c_str(), MaxPlayer, bUsePassword, (bUsePassword ? Password.c_str() : "0"));

			if (mysql_query(m_SessionConnector, Query) == 0) {
				return true;
			}
		}
	}
	return false;
}