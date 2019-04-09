#include "DataBase.h"
#include <iostream>
#include <string>

DataBase::DataBase() {
	mysql_init(&m_SessionHandle);

	try {
		m_SessionConnector = mysql_real_connect(&m_SessionHandle, "127.0.0.1", "root", "a2233212", "session_information", 3306, nullptr, 0);
		if (!m_SessionConnector) {
			throw "Init Mysql Failure!";
		}
	}
	catch (std::string& Exception) {
		std::cout << Exception << std::endl;
	}
}

DataBase::~DataBase() {
	mysql_close(&m_SessionHandle);
}