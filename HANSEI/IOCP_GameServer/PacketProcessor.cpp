#include "IOCP.h"
#include "PacketProcessor.h"
#include "GamePlayPacket.h"
#include <iostream>
#include <algorithm>

PacketProcessor::PacketProcessor(IOCP* Server) : m_Server(Server), m_DataBase(nullptr) {
	m_Processor.push_back(std::bind(&PacketProcessor::JoinGame, this, std::placeholders::_1, std::placeholders::_2));
	m_Processor.push_back(std::bind(&PacketProcessor::UpdatePlayerInformation, this, std::placeholders::_1, std::placeholders::_2));
	m_Processor.push_back(std::bind(&PacketProcessor::DisconnectPlayer, this, std::placeholders::_1, std::placeholders::_2));
	m_Processor.push_back(std::bind(&PacketProcessor::StartGame, this, std::placeholders::_1, std::placeholders::_2));

	m_DataBase = new DataBase;
	if (!m_DataBase) {
		throw "Init DataBase Failure!";
	}
}

PacketProcessor::~PacketProcessor() {
	if (m_DataBase) {
		delete m_DataBase;
		m_DataBase = nullptr;
	}
}

void PacketProcessor::JoinGame(SOCKETINFO* Info, PACKET*& Packet) {
	GAMEPACKET* GamePacket = static_cast<GAMEPACKET*>(Packet);

	if (GamePacket && Info && m_Server && m_DataBase) {
		auto Iterator = m_Sessions.find(GamePacket->m_SessionID);
		if (Iterator != m_Sessions.cend()) {
			if (m_DataBase->TryJoinGame(GamePacket)) {
				GamePacket->m_MessageType = EPACKETMESSAGETYPE::EPMT_NEWPLAYER;
				GamePacket->m_FailedReason = EPACKETFAILEDTYPE::EPFT_SUCCEED;
				GamePacket->m_Socket = Info->m_Socket;

				BroadCast(GamePacket, Iterator->second);
				BroadCast(Iterator->second, GamePacket);

				Iterator->second.push_back(*GamePacket);

				GamePacket->m_MessageType = EPACKETMESSAGETYPE::EPMT_JOIN;
				m_Server->Send(Info, GamePacket);
				std::cout << "Join Session Is Succeed! - " << Info->m_Socket << '\t' << GamePacket->m_PlayerNickName << std::endl;
				return;
			}
			GamePacket->m_MessageType = EPACKETMESSAGETYPE::EPMT_JOIN;
			GamePacket->m_FailedReason = EPACKETFAILEDTYPE::EPFT_FAILED;
			m_Server->Send(Info, GamePacket);
			std::cout << "Join Session Is Failure! - " << Info->m_Socket << '\t' << GamePacket->m_PlayerNickName << std::endl;
		}
		else {
			if (m_DataBase->TryJoinGame(GamePacket)) {
				GamePacket->m_MessageType = EPACKETMESSAGETYPE::EPMT_JOIN;
				GamePacket->m_FailedReason = EPACKETFAILEDTYPE::EPFT_SUCCEED;
				GamePacket->m_Socket = Info->m_Socket;

				std::vector<GAMEPACKET> PlayerList;
				PlayerList.push_back(*GamePacket);
				m_Sessions.insert(std::make_pair(GamePacket->m_SessionID, PlayerList));

				m_Server->Send(Info, GamePacket);
				std::cout << "Join Session Is Succeed! - " << Info->m_Socket << '\t' << GamePacket->m_PlayerNickName << std::endl;
				return;
			}
			GamePacket->m_MessageType = EPACKETMESSAGETYPE::EPMT_JOIN;
			GamePacket->m_FailedReason = EPACKETFAILEDTYPE::EPFT_FAILED;
			m_Server->Send(Info, GamePacket);
			std::cout << "Join Session Is Failure! - " << Info->m_Socket << '\t' << GamePacket->m_PlayerNickName << std::endl;
		}
	}
}

void PacketProcessor::UpdatePlayerInformation(SOCKETINFO* Info, PACKET*& Packet) {
	GAMEPACKET* GamePacket = static_cast<GAMEPACKET*>(Packet);

	if (GamePacket && Info) {
		auto Session = m_Sessions.find(GamePacket->m_SessionID);
		if (Session != m_Sessions.cend()) {
			auto Character = std::find_if(Session->second.begin(), Session->second.end(), [&](const GAMEPACKET& Data) -> bool { if (Data.m_UniqueKey == GamePacket->m_UniqueKey) { return true; } return false; });
			if (Character != Session->second.cend()) {
				(*Character) = (*GamePacket);
				GamePacket->m_MessageType = EPACKETMESSAGETYPE::EPMT_UPDATE;
				GamePacket->m_FailedReason = EPACKETFAILEDTYPE::EPFT_SUCCEED;

				BroadCast(GamePacket, Session->second);
			}
		}
	}
}

void PacketProcessor::DisconnectPlayer(SOCKETINFO* Info, PACKET*& Packet) {
	GAMEPACKET* GamePacket = static_cast<GAMEPACKET*>(Packet);

	if (GamePacket && Info && m_DataBase) {
		if (m_DataBase->TryDisconnectGame(GamePacket->m_SessionID, m_Sessions)) {
			auto Iterator = m_Sessions.find(GamePacket->m_SessionID);
			if (Iterator != m_Sessions.cend()) {
				auto Character = std::find_if(Iterator->second.begin(), Iterator->second.end(), [&](const GAMEPACKET& Data) -> bool { if (Data.m_UniqueKey == GamePacket->m_UniqueKey) { return true; } return false; });
				if (Character != Iterator->second.cend()) {
					Iterator->second.erase(Character);
					// Unique Key Àç¼³Á¤
					GamePacket->m_MessageType = EPACKETMESSAGETYPE::EPMT_DISCONNECTOTHER;
					GamePacket->m_FailedReason = EPACKETFAILEDTYPE::EPFT_SUCCEED;

					BroadCast(GamePacket, Iterator->second);
					std::cout << "Disconnect Session Is Succeed! - " << Info->m_Socket << '\t' << GamePacket->m_SessionID << '\t' << GamePacket->m_UniqueKey << std::endl;
					return;
				}
			}
		}
	}
}

void PacketProcessor::StartGame(SOCKETINFO* Info, PACKET*& Packet) {
	GAMEPACKET* GamePacket = static_cast<GAMEPACKET*>(Packet);

	if (GamePacket && Info && m_DataBase) {
		auto Iterator = m_Sessions.find(GamePacket->m_SessionID);
		if (Iterator != m_Sessions.cend()) {
			GamePacket->m_MessageType = EPACKETMESSAGETYPE::EPMT_STARTGAME;
			GamePacket->m_FailedReason = EPACKETFAILEDTYPE::EPFT_SUCCEED;

			BroadCast(GamePacket, Iterator->second);

			std::cout << "Start Game Succeed! - " << Info->m_Socket << std::endl;
			return;
		}
		GamePacket->m_MessageType = EPACKETMESSAGETYPE::EPMT_STARTGAME;
		GamePacket->m_FailedReason = EPACKETFAILEDTYPE::EPFT_FAILED;
		m_Server->Send(Info, GamePacket);
		std::cout << "Start Game Failure! - " << Info->m_Socket << std::endl;
	}
}

void PacketProcessor::BroadCast(GAMEPACKET*& Packet, const std::vector<struct GAMEPACKET>& PlayerList) {
	SOCKETINFO* Info = new SOCKETINFO;
	memset(Info, 0, sizeof(SOCKETINFO));

	for (auto It : PlayerList) {
		Info->m_Socket = It.m_Socket;
		m_Server->Send(Info, Packet);
	}
}

void PacketProcessor::BroadCast(const std::vector<struct GAMEPACKET>& PlayerList, GAMEPACKET *& Packet) {
	SOCKETINFO* Info = new SOCKETINFO;
	memset(Info, 0, sizeof(SOCKETINFO));
	Info->m_Socket = Packet->m_Socket;

	for (auto It : PlayerList) {
		It.m_MessageType = Packet->m_MessageType;
		m_Server->Send(Info, It);
	}
}