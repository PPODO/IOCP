#include "IOCP_ForMain.h"

int main() {
	IOCP_ForMain IOCP;

	if (IOCP.Init(3510)) {
		IOCP.StartIOCP();
	}
}