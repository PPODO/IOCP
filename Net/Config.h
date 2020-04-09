#pragma once
#include <cstdio>
#include <memory>
#include <Windows.h>
#include <exception>

class CConfig {
	static const size_t buffer_length = 32;
	friend std::shared_ptr<const CConfig> LoadIni(const char* const filePath) noexcept;
private:
	CConfig() noexcept : 
		mMaxThreadCount(), mMaxClientCount(),
		mPortNumber(), mProtocolType() {
		ZeroMemory(mIPAddress, buffer_length);
	}

public:
	int mMaxThreadCount;
	int mMaxClientCount;
	int mDefaultBufferSize;

public:
	char mIPAddress[buffer_length];
	int mPortNumber;
	int mProtocolType;

};

static std::shared_ptr<const CConfig> LoadIni(const char* const filePath) noexcept {
	CConfig* Result = nullptr;
	try {
		Result = new CConfig;

		Result->mMaxThreadCount = GetPrivateProfileInt("System", "Max Thread Count", 0, filePath);
		Result->mMaxClientCount = GetPrivateProfileInt("System", "Max Client Count", 256, filePath);
		Result->mDefaultBufferSize = GetPrivateProfileInt("System", "Default Buffer Size", 2048, filePath);

		GetPrivateProfileString("Networking", "IP Address", "0.0.0.0", Result->mIPAddress, CConfig::buffer_length, filePath);
		Result->mPortNumber = GetPrivateProfileInt("Networking", "Port Number", 3550, filePath);
		Result->mProtocolType = GetPrivateProfileInt("Networking", "Protocol Type", 0, filePath);

	}
	catch (const std::exception& Exception) {
		printf("Exception - %s", Exception.what());
	}
	return std::shared_ptr<CConfig>(Result);
}