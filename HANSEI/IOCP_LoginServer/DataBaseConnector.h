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

class DataBaseConnector : public DATABASE::DataBaseConnectorClass {
private:
	EFAILED AddPlayerToSession(std::string& SessionName, int CurrentPlayer);
	
public:
	int GetAllSessionInformation(SessionInformation& Sessions, int MinLimit, int&);
	bool GetNickNameByID(std::string&, std::string&);
	bool CheckIfUserExists(std::string&, std::string&);
	EFAILED InsertNewAccount(std::string& NickName, std::string& ID, std::string& Password);
	EFAILED InsertNewSession(const class Session& _Session);
	EJOINFAILED JoinSession(std::string& SessionName, bool UsePassword, std::string& Password);

};