#include "mysql_connect.h"

mysql_connect::mysql_connect(mysql_pool* pool) : mConnection(nullptr), mPool(pool), mbIsSuccess(false) {
	mConnection = mPool->GetOneConnection();
	BeginTransaction();
}

mysql_connect::mysql_connect(const std::string& dbName) : mConnection(nullptr), mPool(nullptr), mbIsSuccess(false) {
	mPool = mysql_manager::GetDBPool(dbName);
	mConnection = mPool->GetOneConnection();
	BeginTransaction();
}

mysql_connect::~mysql_connect() {
	if (mbIsSuccess)
		Commit();
	else
		Rollback();
	
	mPool->Release(mConnection);
}

void mysql_connect::BeginTransaction() {
	mysql_query(mConnection, "START TRANSACTION");
}

void mysql_connect::Commit() {
	mysql_query(mConnection, "COMMIT");
}

void mysql_connect::Rollback() {
	mysql_query(mConnection, "ROLLBACK");
}