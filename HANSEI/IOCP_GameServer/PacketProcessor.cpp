#include "IOCP.h"
#include "PacketProcessor.h"

PacketProcessor::PacketProcessor(IOCP* Server) : m_Server(Server), m_DataBase(nullptr) {
	m_Processor.push_back(std::bind(&PacketProcessor::JoinGame, this, std::placeholders::_1, std::placeholders::_2));

	m_DataBase = new DataBase;
}

PacketProcessor::~PacketProcessor() {
	if (m_DataBase) {
		delete m_DataBase;
		m_DataBase = nullptr;
	}
}

void PacketProcessor::JoinGame(SOCKETINFO* Info, GAMEPACKET*& Packet) {
	if (Info && Packet && m_Server && m_DataBase) {
		Packet->m_MessageType = EPACKETMESSAGETYPE::EPMT_JOIN;
		Packet->m_FailedReason = EPACKETFAILEDTYPE::EPFT_SUCCEED;

		m_Server->Send(Info, Packet);
	}
}