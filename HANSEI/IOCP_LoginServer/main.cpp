#include "IOCP.h"

int main() {
	IOCP Server(3515);

	if (Server.Init()) {
		Server.Start();
	}
	return 0;
}