#include "DataBase.h"
#include "GamePlayPacket.h"
#include <iostream>
#include <sstream>
#include <string>

DataBase::DataBase() {
	mysql_init(&m_SessionHandle);

	try {
		m_SessionConnector = mysql_real_connect(&m_SessionHandle, "127.0.0.1", "root", "a2233212", "session_information", 3306, nullptr, 0);
		if (!m_SessionConnector) {
			throw "Init Mysql Failure!";
		}
	}
	catch (std::string& Exception) {
		std::cout << Exception << std::endl;
	}
}

DataBase::~DataBase() {
	mysql_close(&m_SessionHandle);
}

bool DataBase::TryJoinGame(GAMEPACKET*& Packet) {
	if (Packet) {
		char Query[MaxQuerySize] = { "\0" };
		sprintf(Query, "SELECT* FROM `information` WHERE `session_id` = '%d'", Packet->m_SessionID);

		if (mysql_query(m_SessionConnector, Query) == 0) {
			MYSQL_RES* Result = mysql_store_result(m_SessionConnector);
			if (Result) {
				MYSQL_ROW Rows = nullptr;
				while ((Rows = mysql_fetch_row(Result))) {
					Session SessionInformation;
					std::stringstream Stream;
					for (size_t i = 0; i < Result->field_count; i++) {
						Stream << Rows[i] << std::endl;
					}
					Stream >> SessionInformation;

					memset(Query, 0, MaxQuerySize);
					sprintf(Query, "UPDATE `information` SET `current_player` = '%d' WHERE `session_id` = '%d'", SessionInformation.m_CurrentPlayer + 1, Packet->m_SessionID);

					if (mysql_query(m_SessionConnector, Query) == 0) {
						mysql_free_result(Result);
						return true;
					}
				}
				mysql_free_result(Result);
			}
		}
	}
	return false;
}

bool DataBase::TryDisconnectGame(const int& SessionID, std::map<int, std::tuple<int, std::vector<struct GAMEPACKET>>>& SessionList) {
	char Query[MaxQuerySize] = { "\0" };
	sprintf(Query, "SELECT* FROM `information` WHERE `session_id` = '%d'", SessionID);

	if (mysql_query(m_SessionConnector, Query) == 0) {
		MYSQL_RES* Result = mysql_store_result(m_SessionConnector);
		if (Result) {
			MYSQL_ROW Rows = nullptr;
			while ((Rows = mysql_fetch_row(Result))) {
				std::stringstream Stream;
				Session SessionInformation;
				for (size_t i = 0; i < Result->field_count; i++) {
					Stream << Rows[i] << std::endl;
				}
				Stream >> SessionInformation;

				memset(Query, 0, MaxQuerySize);
				if (SessionInformation.m_CurrentPlayer - 1 > 0) {
					sprintf(Query, "UPDATE `information` SET `current_player` = '%d' WHERE `session_id` = '%d'", SessionInformation.m_CurrentPlayer - 1, SessionID);
					
					if (mysql_query(m_SessionConnector, Query) == 0) {
						mysql_free_result(Result);
						return true;
					}
				}
				else {
					sprintf(Query, "DELETE FROM `information` WHERE `session_id` = '%d'", SessionID);
					SessionList.erase(SessionID);
					if (mysql_query(m_SessionConnector, Query) == 0) {
						mysql_free_result(Result);
						return false;
					}
				}
			}
			mysql_free_result(Result);
		}
	}
	return false;
}