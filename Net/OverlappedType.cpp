#include "OverlappedType.h"

namespace Overlapped {
	void DeleteIOContext(Base_Overlapped* const overlapped) {
		switch (overlapped->mFlag) {
		case EOverlappedFlag::Accept:
			delete static_cast<Accept_Overlapped*>(overlapped);
			break;
		case EOverlappedFlag::Disconnect:

			break;
		case EOverlappedFlag::Receive:

			break;
		case EOverlappedFlag::Send:
			delete static_cast<Send_Overlapped*>(overlapped);
			break;
		case EOverlappedFlag::TryDisconnect:
			
			break;
		}
	}

}