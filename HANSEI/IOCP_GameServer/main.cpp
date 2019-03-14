#include "IOCP.h"

int main() {
	IOCP Server;

	if (Server.Init(3500)) {
		Server.StartIOCP();
	}
}