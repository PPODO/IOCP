#include "Networking.h"

SESSION::Session::Session() {
}

SESSION::Session::~Session() {
}

SESSION::SessionInformation::SessionInformation() {
}

SESSION::SessionInformation::~SessionInformation() {
}

DATABASE::DataBaseConnectorClass::DataBaseConnectorClass() {
	mysql_init(&m_LoginConnector);
	mysql_init(&m_SessionConnector);
	try {
		m_LoginHandle = mysql_real_connect(&m_LoginConnector, "127.0.0.1", "root", "a2233212", "user_information", 3306, (char*)nullptr, 0);
		m_SessionHandle = mysql_real_connect(&m_SessionConnector, "127.0.0.1", "root", "a2233212", "session_information", 3306, (char*)nullptr, 0);
		if (!m_LoginHandle) { throw "MySQL Connect Failure!!\n"; }
		if (!m_SessionHandle) { throw "MySQL Connect Failure!!\n"; }
	}
	catch (std::exception& Exception) {
		std::cout << Exception.what() << std::endl;
	}
}

DATABASE::DataBaseConnectorClass::~DataBaseConnectorClass() {
	mysql_close(m_SessionHandle);
	mysql_close(m_LoginHandle);
}

PLAYER::CharacterInformation::CharacterInformation() {
}

PLAYER::CharacterInformation::~CharacterInformation() {
}

PLAYER::Character::Character() {
}

PLAYER::Character::~Character() {
}
