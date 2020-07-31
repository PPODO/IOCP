#include "mysql_manager.h"

void mysql_manager::AddDB(const std::string& host, const std::string& id, const std::string& password, const std::string& databaseName, unsigned short portNumber, unsigned short maxConnection) {
	if (mDB.count(databaseName))
		return;
	mDB.insert(std::make_pair(databaseName, new mysql_pool(host, id, password, databaseName, portNumber, maxConnection)));
}

mysql_pool* mysql_manager::GetDBPool(const std::string& dbName) {
	auto iterator = mDB.find(dbName);
	if (iterator != mDB.cend())
		return iterator->second;
	return nullptr;
}