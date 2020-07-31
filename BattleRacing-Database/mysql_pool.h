#pragma once
#pragma comment(lib, "libmysql.lib")

#include <mysql.h>
#include <string>
#include <queue>
#include "../BattleRacing-Server/critical_section.h"

class mysql_pool {
public:
	mysql_pool(
		const std::string& host, 
		const std::string& id, 
		const std::string& password, 
		const std::string& databaseName, 
		unsigned short portNumber, 
		unsigned short maxConnection);
	~mysql_pool();

public:
	MYSQL* GetOneConnection();
	void Release(MYSQL* connection);

private:
	MYSQL* CreateConnection();
	bool IsEmpty();

private:
	critical_section mCriticalSection;

	std::queue<MYSQL*> mMysqlPool;

	std::string mServerAddress;
	std::string mID;
	std::string mPassword;
	std::string mDatabaseName;
	unsigned short mPortNumber;

	unsigned short mCurrentConnection;
	unsigned short mMaxConnection;

};