#pragma once
#include "mysql_header.h"
#include <unordered_map>

class mysql_manager {
public:
	static void AddDB(
		const std::string& host,
		const std::string& id,
		const std::string& password,
		const std::string& databaseName,
		unsigned short portNumber,
		unsigned short maxConnection);

	static mysql_pool* GetDBPool(const std::string& dbName);

private:
	static std::unordered_map<std::string, mysql_pool*> mDB;

};