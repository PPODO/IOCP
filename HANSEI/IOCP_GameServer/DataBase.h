#pragma once
#pragma comment(lib, "libmysql.lib")
#include <mysql.h>

class DataBase {
private:
	MYSQL m_SessionHandle;
	MYSQL* m_SessionConnector;

public:
	DataBase();
	~DataBase();

public:


};