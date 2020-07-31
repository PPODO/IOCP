#include "mysql_pool.h"
#include <iostream>

mysql_pool::mysql_pool(
	const std::string& host,
	const std::string& id,
	const std::string& password,
	const std::string& databaseName,
	unsigned short portNumber,
	unsigned short maxConnection)
	: mServerAddress(host), mID(id), mPassword(password)
	, mDatabaseName(databaseName), mPortNumber(portNumber), mMaxConnection(maxConnection), mCurrentConnection(0) {}

mysql_pool::~mysql_pool() {
	while (mMysqlPool.size() != 0) {
		mysql_close(mMysqlPool.front());
		mMysqlPool.pop();
	}
}

MYSQL* mysql_pool::GetOneConnection() {
	mCriticalSection.Lock();
	MYSQL* connection;
	if (!IsEmpty()) {
		while (!IsEmpty() && mysql_ping(mMysqlPool.front())) {
			mysql_close(mMysqlPool.front());
			mMysqlPool.pop();
			mCurrentConnection--;
		}

		if (!IsEmpty()) {
			connection = mMysqlPool.front();
			mMysqlPool.pop();
		}
		else if(mCurrentConnection < mMaxConnection) {
			connection = CreateConnection();
		}
		else {
			std::cout << "the mysql connection is too much!\n";
		}
	}
	else if (mCurrentConnection < mMaxConnection) {
		connection = CreateConnection();
	}
	else {
		std::cout << "the mysql connection is too much!\n";
	}
	mCriticalSection.Unlock();
	return connection;
}

void mysql_pool::Release(MYSQL* connection) {
	if (connection != NULL) {
		mCriticalSection.Lock();
		mMysqlPool.push(connection);
		mCriticalSection.Unlock();
	}
}

MYSQL* mysql_pool::CreateConnection() {
	mCriticalSection.Lock();
	MYSQL* result = NULL;
	result = mysql_init(result);
	if (result != NULL) {
		unsigned short timeOut = 10;
		mysql_options(result, MYSQL_OPT_CONNECT_TIMEOUT, &timeOut);
		if (mysql_real_connect(result, mServerAddress.c_str(), mID.c_str(),mPassword.c_str(),mDatabaseName.c_str(),mPortNumber, "", 2)) {
			mCurrentConnection++;
		}
		else {
			std::cout << mysql_error(result) << std::endl;
		}
	}
	else {
		std::cout << "Init Failure!\n";
	}
	mCriticalSection.Unlock();
	return result;
}

bool mysql_pool::IsEmpty() {
	return !mMysqlPool.empty();
}
