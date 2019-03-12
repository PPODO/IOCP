#include "IOCP.h"

int main() {
	IOCP Server;

	if (Server.Init(3510)) {
		Server.StartIOCP();
	}
}