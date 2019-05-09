#include "IOCP.h"
#include <boost/asio.hpp>

int main() {
	IOCP Server(3500);

	if (Server.Init()) {
		Server.Start();
	}
	return 0;
}