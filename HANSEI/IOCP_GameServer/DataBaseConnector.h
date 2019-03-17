#pragma once
#pragma comment(lib, "HanseiDLL.lib")
#include "Networking.h"

using namespace SESSION;
using namespace PACKET;
static const int MaxQueryLen = 256;

class DataBaseConnector : public DATABASE::DataBaseConnectorClass {
public:

public:
	DataBaseConnector();
	virtual ~DataBaseConnector() override;

public:
	EFAILED AddPlayerToSession(std::string& SessionName);
	bool CompareSessionName(const std::string& SessionName);
	void RemoveSession(const std::string& SessionName);
	void RemovePlayerFromSession(const std::string& SessionName);

};