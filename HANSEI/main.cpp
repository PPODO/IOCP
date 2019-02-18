#include "IOCP.h"

int main() {
	IOCP IOCPServer;

	if (IOCPServer.Init()) {
		IOCPServer.StartIOCP();
	}
	return 0;
}