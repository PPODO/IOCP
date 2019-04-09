#include "IOCP.h"

int main() {
	IOCP Server(3500);

	if (Server.Init()) {
		Server.Start();
	}
	return 0;
}