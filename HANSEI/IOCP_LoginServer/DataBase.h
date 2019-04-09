#pragma once
#pragma comment(lib, "libmysql.lib")
#include "SessionInformation.h"
#include <string>
#include <mysql.h>

static const size_t MaxQuerySize = 256;

class DataBase {
private:
	MYSQL m_LoginHandle, m_SessionHandle;
	MYSQL* m_LoginConnector, *m_SessionConnector;

public:
	DataBase();
	~DataBase();

public:
	bool TryLogin(const std::string& ID, const std::string& Password, std::string& UserName);
	bool IsItExistUserInfo(const std::string& ID, const std::string& Password, const std::string& UserName);
	bool IsItExistSessionInfo(const std::string& SessionName);
	bool GetSessionInformation(SessionInformation& Information, const size_t& CurrentPage, size_t& SessionCount);
	bool TryJoinSession(const std::string& SessionName, const bool& UsePassword, const std::string& Password);

public:
	bool InsertNewAccount(const std::string& UserName, const std::string& ID, const std::string& Password);
	bool InsertNewSession(const std::string& SessionName, const int& MaxPlayer, const bool& bUsePassword, const std::string& Password);

};