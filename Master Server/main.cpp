#include <iostream>
#include <Net/MemoryPool.hpp>

class A : public MemoryManagement::CMemoryPool<A, 1> {
public:
	A(int a, float b, char c) : mA(a), mB(b), mC(c) {};

public:
	int mA;
	float mB;
	char mC;

};

int main() {


	return 0;
}