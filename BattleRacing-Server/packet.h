#pragma once
#include <string>
#include <unordered_map>

enum class EPacketType : uint8_t {
	None,
	Login,
	SignUp,
	Connected,
};

struct FVectorParam {
public:
	FVectorParam() : mX(), mY(), mZ() {};
	FVectorParam(float x) : mX(x), mY(0.f), mZ(0.f) {};
	FVectorParam(float x, float y) : mX(x), mY(y), mZ(0.f) {};
	FVectorParam(float x, float y, float z) : mX(x), mY(y), mZ(z) {};

public:
	template<typename archive>
	void serialize(archive& ar) {
		ar& mX;
		ar& mY;
		ar& mZ;
	}

public:
	float mX;
	float mY;
	float mZ;

};

class FPacketBase {
	enum EResult { Succeeded, Failure };
public:
	FPacketBase() : mPacketType(EPacketType::None), mResult(-1), mErrorCode(-1) {};
	FPacketBase(EPacketType packetType) : mPacketType(packetType), mResult(0), mErrorCode(0) {};
	FPacketBase(EPacketType packetType, int8_t result, int8_t errorCode) : mPacketType(packetType), mResult(result), mErrorCode(errorCode) {};

public:
	template<typename archive>
	void serialize(archive& ar) {
		ar& reinterpret_cast<uint8_t&>(mPacketType);
		ar& mResult;
		ar& mErrorCode;
	}

public:
	EPacketType mPacketType;
	int8_t mResult;
	int8_t mErrorCode;

};

struct FLoginRequest : public FPacketBase {
public:
	FLoginRequest(const std::string& id, const std::string& password)
		: FPacketBase(EPacketType::Login), mId(id), mPassword(password) {
	};
	FLoginRequest(const char* id, const char* password)
		: FPacketBase(EPacketType::Login), mId(id), mPassword(password) {
	};

public:
	template<typename archive>
	void serialize(archive& ar) {
		FPacketBase::serialize(ar);

		ar& mId;
		ar& mPassword;
	}

public:
	std::string mId;
	std::string mPassword;

};

struct FConnectRequestResult : public FPacketBase {
	enum class ELoginErrorCode : int8_t { Invalid_id, Invalid_password, Nonexist };
public:
	FConnectRequestResult(const std::string& nickname)
		: FPacketBase(EPacketType::Connected), mNickname(nickname) {};
	FConnectRequestResult(const std::string& nickname, int8_t result, int8_t errorCode)
		: FPacketBase(EPacketType::Connected, result, errorCode), mNickname(nickname) {};

public:
	template<typename archive>
	void serialize(archive& ar) {
		FPacketBase::serialize(ar);

		ar& mNickname;
		ar& mVehicleColor;
		ar& mAbilityColor;
	}

public:
	std::string mNickname;
	FVectorParam mVehicleColor;
	FVectorParam mAbilityColor;

};