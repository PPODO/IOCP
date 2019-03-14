#pragma once
#pragma comment(lib, "HanseiDLL.lib")
#include "Networking.h"

static const int MaxQueryLen = 256;

class DataBaseConnector : public DATABASE::DataBaseConnectorClass {
private:

public:
	DataBaseConnector();
	virtual ~DataBaseConnector() override;

public:
	bool CompareSessionName(const std::string& SessionName);
	void RemoveSession(const std::string& SessionName);
	void RemovePlayerFromSession(const std::string& SessionName);

};