#pragma once
#include "packet.h"
#include "memorypool.h"
#include <type_traits>
#include <unordered_map>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <array>

template<typename ..._type>
struct is_container_helper {};

template<typename _type>
using is_containing_stl_container = is_container_helper<typename _type::iterator, typename _type::const_iterator, decltype(std::declval<_type>().begin()), decltype(std::declval<_type>().end()), decltype(std::declval<_type>().cbegin()), decltype(std::declval<_type>().cend())>;

template<typename _type, typename _ = void>
struct is_container : public std::false_type {};

template<typename _type>
struct is_container<_type, typename std::conditional_t<false, is_containing_stl_container<_type>, void>> : public std::true_type {};

template<typename archive>
class oserializer {
public:
	template<typename _type>
	archive& operator<<(_type& data) {
		data.serialize(*this->This());
		return *this->This();
	}

	// is class
	template<typename _type, typename std::enable_if_t<std::is_class<_type>::value & !is_container<_type>::value>* = nullptr>
	void operator&(_type& data) {
		data.serialize(*this->This());
	}

	// is stl container
	template<typename _type, typename std::enable_if_t<std::is_class<_type>::value& is_container<_type>::value>* = nullptr>
	void operator&(_type& data) {
		this->This()->Write(data.size());

		for (auto& iterator : data) {
			(*this->This())& iterator;
		}
	}
	
	// is pointer
	template<typename _type>
	void operator&(_type* data) {
		(*this->This())& (*data);
	}

	// is arithmetic
	template<typename _type, typename std::enable_if_t<std::is_arithmetic<_type>::value>* = nullptr>
	void operator&(_type& data) {
		This()->Write(data);
	}

private:
	inline archive* This() {
		return static_cast<archive*>(this);
	}

};

template<typename archive>
class iserializer {
public:
	template<typename _type>
	archive& operator>>(_type& data) {
		data.serialize(*this->This());
		return *this->This();
	}

	// is class
	template<typename _type, typename std::enable_if_t<std::is_class<_type>::value & !is_container<_type>::value, _type>* = nullptr>
	void operator&(_type& data) {
		data.serialize(*this->This());
	}

	// is stl container
	template<typename _type, typename std::enable_if_t<std::is_class<_type>::value & is_container<_type>::value, _type>* = nullptr>
	void operator&(_type& data) {
		size_t stl_length = 0;

		this->This()->Read(stl_length);
		data.resize(stl_length);

		for (auto& iterator : data) {
			(*this->This())& iterator;
		}
	}

	// is pointer
	template<typename _type>
	void operator&(_type* data) {
		(*this->This())& (*data);
	}

	// is arithmetic
	template<typename _type, typename std::enable_if_t<std::is_arithmetic<_type>::value, _type>* = nullptr>
	void operator&(_type& data) {
		this->This()->Read(data);
	}

private:
	inline archive* This() {
		return static_cast<archive*>(this);
	}

};

struct buffer : public memory_pool<buffer> {
public:
	static const size_t buffer_length = 2048;

public:
	buffer() { std::memset(mBuffer, 0, buffer_length); }
	buffer(const char* str) { strncpy_s(mBuffer, str, buffer_length); }

	char& operator[](size_t index) {
		if (index >= buffer_length) { return mBuffer[buffer_length - 1]; }
		return mBuffer[index];
	}

	size_t Len() {
		return strlen(mBuffer);
	}

public:
	char mBuffer[buffer_length];

};

class OBitStream : public oserializer<OBitStream> {
	friend class IBitStream;
public:
	OBitStream() : mBuffer(new buffer()), mBitHead(0) {}
	~OBitStream() {};

public:
	template<typename _type>
	void Write(const _type& data) {
		const char* srcByte = reinterpret_cast<const char*>(&data);
		size_t bitCount = sizeof(data) * 8;

		while (bitCount > 8) {
			WriteBits(*srcByte, 8);
			++srcByte;
			bitCount -= 8;
		}

		if (bitCount > 0)
			WriteBits(*srcByte, bitCount);
	}

public:
	buffer& GetBuffer() {
		return *mBuffer;
	}

private:
	template<typename _type>
	void WriteBits(const _type& data, size_t bitCount) {
		size_t nextBit = mBitHead + bitCount;

		uint32_t byteOffset = mBitHead / 8;
		uint32_t bitOffset = mBitHead % 8;

		uint32_t currentMask = ~(0xff << bitOffset);

		(*mBuffer)[byteOffset] = (((*mBuffer)[byteOffset] & currentMask) | (data << bitOffset));

		uint32_t processedBits = 8 - bitOffset;
		if (processedBits < bitCount) {
			(*mBuffer)[byteOffset + 1] = (data >> processedBits);
		}
		mBitHead = nextBit;
	}

private:
	static const size_t buffer_length = 2048;

	std::shared_ptr<buffer> mBuffer;
	size_t mBitHead;

};

class IBitStream : public iserializer<IBitStream> {
public:
	IBitStream(const char* stream) : mBuffer(new buffer(stream)), mBitHead(0) {};
	IBitStream(const std::string& stream) : mBuffer(new buffer(stream.c_str())), mBitHead(0) {};
	IBitStream(const OBitStream& oStream) : mBuffer(oStream.mBuffer), mBitHead(0) {};
	~IBitStream() {};

public:
	template<typename _type>
	void Read(_type& data) {
		char* srcByte = reinterpret_cast<char*>(&data);
		size_t bitCount = sizeof(data) * 8;

		while (bitCount > 8) {
			ReadBits(*srcByte, 8);
			++srcByte;
			bitCount -= 8;
		}

		if (bitCount > 0)
			ReadBits(*srcByte, bitCount);
	}

public:
	buffer& GetBuffer() {
		return *mBuffer;
	}

private:
	template<typename _type>
	void ReadBits(_type& data, size_t bitCount) {
		uint32_t byteOffset = mBitHead / 8;
		uint32_t bitOffset = mBitHead % 8;

		data = (*mBuffer)[byteOffset] >> bitOffset;

		uint32_t processedBits = 8 - bitOffset;
		if (processedBits < bitCount) {
			data |= ((*mBuffer)[byteOffset + 1] << processedBits);
		}

		data &= ~(0xff << bitCount);
		mBitHead += bitCount;
	}

private:
	const std::shared_ptr<buffer> mBuffer;
	size_t mBitHead;

};