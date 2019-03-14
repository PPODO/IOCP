#include "DataBaseConnector.h"

DataBaseConnector::DataBaseConnector() {

}

DataBaseConnector::~DataBaseConnector() {

}

bool DataBaseConnector::CompareSessionName(const std::string & SessionName) {
	char Query[MaxQueryLen] = { "\0" };

	sprintf(Query, "SELECT * FROM `information` WHERE session_name = '%s'", SessionName.c_str());

	if (mysql_query(m_SessionHandle, Query) == 0) {
		MYSQL_RES* Result = mysql_store_result(m_SessionHandle);
		if (Result) {
			MYSQL_ROW Row = nullptr;
			while ((Row = mysql_fetch_row(Result))) {
				SESSION::Session NewSession;
				std::stringstream Stream;
				for (int i = 0; i < Result->field_count; i++) {
					Stream << Row[i] << std::endl;
				}
				Stream >> NewSession;
				if (NewSession.m_SessionName.compare(SessionName)) {
					mysql_free_result(Result);
					return false;
				}
				mysql_free_result(Result);
				return true;
			}
		}
		mysql_free_result(Result);
		return false;
	}
	return true;
}

void DataBaseConnector::RemoveSession(const std::string& SessionName) {
	char Query[MaxQueryLen] = { "\0" };

	sprintf(Query, "DELETE FROM `information` WHERE session_name = '%s'", SessionName.c_str());

	if (mysql_query(m_SessionHandle, Query) == 0) {
		return;
	}
	return;
}

void DataBaseConnector::RemovePlayerFromSession(const std::string& SessionName) {
	char Query[MaxQueryLen] = { "\0" };

	sprintf(Query, "SELECT * FROM `information` WHERE session_name = '%s'", SessionName.c_str());

	if (mysql_query(m_SessionHandle, Query) == 0) {
		MYSQL_RES* Result = mysql_store_result(m_SessionHandle);
		if (Result) {
			SESSION::Session SessionInfo;
			MYSQL_ROW Row = nullptr;
			while ((Row = mysql_fetch_row(Result))) {
				std::stringstream Stream;
				for (int i = 0; i < Result->field_count; i++) {
					Stream << Row[i] << std::endl;
				}
				Stream >> SessionInfo;
			}

			memset(Query, 0, MaxQueryLen);
			sprintf(Query, "UPDATE `information` SET current_player = '%d' WHERE session_name = '%s'", SessionInfo.m_CurrentPlayer - 1, SessionName.c_str());
			if (mysql_query(m_SessionHandle, Query) == 0) {

			}
		}
		mysql_free_result(Result);
	}
}