#include <iostream>
#include <Net/Iocp.h>
#include <Net/CircularBuffer.hpp>

int main() {
	LoadIni("../test.ini");

	IOCP::CIOCP iocp;

	iocp.Initialize();
	iocp.Run();
	return 0;
}