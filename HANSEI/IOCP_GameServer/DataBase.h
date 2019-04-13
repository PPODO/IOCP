#pragma once
#pragma comment(lib, "libmysql.lib")
#include <mysql.h>
#include <vector>
#include <map>

static const size_t MaxQuerySize = 256;

class DataBase {
private:
	MYSQL m_SessionHandle;
	MYSQL* m_SessionConnector;

public:
	DataBase();
	~DataBase();

public:
	bool TryJoinGame(struct GAMEPACKET*& Packet);
	bool TryDisconnectGame(const int& SessionID, std::map<int, std::vector<struct GAMEPACKET>>& SessionList);

};