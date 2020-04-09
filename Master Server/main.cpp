#include <iostream>
#include <Net/Iocp.h>

int main() {
	IOCP::CIOCP iocp(LoadIni("../test.ini"));

	iocp.Initialize();
	iocp.Run();

	return 0;
}