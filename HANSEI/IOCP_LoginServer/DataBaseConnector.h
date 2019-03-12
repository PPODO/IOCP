#pragma once
#pragma comment(lib, "libmysql.lib")
#pragma comment(lib, "HanseiDLL.lib")
#include "Networking.h"
#include <mysql.h>
#include <sstream>
#include <string>

using namespace SESSION;
using namespace PACKET;

static const int MaxQueryLen = 256;

class DataBaseConnector {
private:
	MYSQL m_LoginConnecter;
	MYSQL m_SessionConnecter;
	MYSQL* m_LoginHandler;
	MYSQL* m_SessionHandler;

public:
	DataBaseConnector();
	~DataBaseConnector();

private:
	EFAILED AddPlayerToSession(std::string& SessionName, int CurrentPlayer);
	
public:
	int GetAllSessionInformation(SessionInformation& Sessions);
	bool GetNickNameByID(std::string&, std::string&);
	bool CheckIfUserExists(std::string&, std::string&);
	EFAILED InsertNewAccount(std::string& NickName, std::string& ID, std::string& Password);
	EFAILED InsertNewSession(const class Session& _Session);
	EJOINFAILED JoinSession(std::string& SessionName, bool UsePassword, std::string& Password);

};