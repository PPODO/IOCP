#pragma once
#include <cstdio>
#include <memory>
#include <Windows.h>
#include <exception>

class CConfig {
	friend CConfig& GetNetConfig();
private:
	CConfig() noexcept {
		ZeroMemory(mIPAddress, buffer_length);
	}
	
public:
	static const size_t buffer_length = 32;

public:
	int mMaxThreadCount;
	int mMaxClientCount;
	int mDefaultBufferSize;

public:
	char mIPAddress[buffer_length];
	int mPortNumber;
	int mProtocolType;

};

__forceinline CConfig& GetNetConfig() {
	static CConfig ret;
	return ret;
}

static void LoadIni(const char* const filePath) noexcept {
	auto& Config = GetNetConfig();
	Config.mMaxThreadCount = GetPrivateProfileInt("System", "Max Thread Count", 0, filePath);
	Config.mMaxClientCount = GetPrivateProfileInt("System", "Max Client Count", 256, filePath);
	Config.mDefaultBufferSize = GetPrivateProfileInt("System", "Default Buffer Size", 2048, filePath);

	GetPrivateProfileString("Networking", "IP Address", "0.0.0.0", Config.mIPAddress, CConfig::buffer_length, filePath);
	Config.mPortNumber = GetPrivateProfileInt("Networking", "Port Number", 3550, filePath);
	Config.mProtocolType = GetPrivateProfileInt("Networking", "Protocol Type", 0, filePath);
}