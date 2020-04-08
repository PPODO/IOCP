#include <iostream>

/*template<size_t buffer_size = 512>
class CCircularBuffer {
public:
	CCircularBuffer() : mBufferEnd(mBuffer + (buffer_size - 1)), mBufferFront(mBuffer), mBufferRear(mBuffer) {
		std::memset(mBuffer, 0, buffer_size * sizeof(char));
	}

public:
	void Push(const char* const data, size_t length) {
		if (length > buffer_size - 1) {
			std::cout << "length can be greater than buffer_size!";
			return;
		}

		if (length <= mBufferEnd - mBufferRear - 1) {
			std::memcpy(mBufferRear, data, length);
			mBufferRear += length;
		}
		else {
			int remainBytes = mBufferEnd - mBufferRear;
			std::memcpy(mBufferRear, data, remainBytes);
			mBufferRear = mBuffer;
			std::memcpy(mBufferRear + remainBytes, data, length - remainBytes);
			mBufferRear += (length - remainBytes);
		}
	}

	void Pop(char* const buffer, size_t length) {
		if (length > buffer_size - 1) {
			std::cout << "length can be less than buffer_size!";
			return;
		}

		if (length <= mBufferEnd - mBufferFront - 1) {
			std::memcpy(buffer, mBufferFront, length);
			mBufferFront += length;
		}
		else {
			int remainBytes = mBufferEnd - mBufferFront;
			std::memcpy(buffer, mBufferFront, remainBytes);
			mBufferFront = mBuffer;
			std::memcpy(buffer, mBufferFront, length - remainBytes);
			mBufferFront += length - remainBytes;
		}
	}


private:
	char mBuffer[buffer_size];
	char* const mBufferEnd;
	char* mBufferFront;
	char* mBufferRear;

};*/

int main() {

	return 0;
}