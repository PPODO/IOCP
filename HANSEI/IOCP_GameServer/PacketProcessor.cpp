#include "IOCP.h"
#include "PacketProcessor.h"
#include "GamePlayPacket.h"
#include <iostream>
#include <algorithm>

PacketProcessor::PacketProcessor(IOCP* Server) : m_Server(Server), m_DataBase(nullptr), m_bIsStop(false) {
	m_RandomAlgorithm = std::mt19937_64(m_RandomDevice());

	m_Processor.push_back(std::bind(&PacketProcessor::JoinGame, this, std::placeholders::_1, std::placeholders::_2));
	m_Processor.push_back(std::bind(&PacketProcessor::UpdatePlayerInformation, this, std::placeholders::_1, std::placeholders::_2));
	m_Processor.push_back(std::bind(&PacketProcessor::DisconnectPlayer, this, std::placeholders::_1, std::placeholders::_2));
	m_Processor.push_back(std::bind(&PacketProcessor::StartGame, this, std::placeholders::_1, std::placeholders::_2));
	m_Processor.push_back(std::bind(&PacketProcessor::ChangeReadyState, this, std::placeholders::_1, std::placeholders::_2));
	m_Processor.push_back(std::bind(&PacketProcessor::AddNewSpawnTimer, this, std::placeholders::_1, std::placeholders::_2));

	m_DataBase = new DataBase;
	if (!m_DataBase) {
		throw "Init DataBase Failure!";
	}

	m_TimerThread = std::thread([this]() {
		while (!m_bIsStop) {
			std::unique_lock<std::mutex> Lock(m_Lock);
			if (!m_TimerList.empty()) {
				for (auto Iterator = m_TimerList.begin(); Iterator != m_TimerList.end();) {
					if (std::get<ETP_PACKET>(*Iterator)) {
						if (std::chrono::system_clock::now() - std::get<ETP_LASTTIME>(*Iterator) > std::get<ETP_DELAY>(*Iterator)) {
							auto Session = m_Sessions.find(std::get<ETP_SESSIONID>(*Iterator));
							if (Session != m_Sessions.cend()) {
								BroadCast(std::get<ETP_PACKET>(*Iterator), std::get<ESI_PLAYERINFORMATION>(Session->second));
							}
							delete std::get<ETP_PACKET>(*Iterator);
							Iterator = m_TimerList.erase(Iterator);
							continue;
						}
					}
					else {
						delete std::get<ETP_PACKET>(*Iterator);
						Iterator = m_TimerList.erase(Iterator);
						continue;
					}
					++Iterator;
				}
			}
		}
	});
}

PacketProcessor::~PacketProcessor() {
	if (m_DataBase) {
		delete m_DataBase;
		m_DataBase = nullptr;
	}

	m_bIsStop = true;
	m_TimerThread.join();
}

void PacketProcessor::JoinGame(SOCKETINFO* Info, GAMEPACKET*& Packet) {
	if (Packet && Info && m_Server && m_DataBase) {
		auto Iterator = m_Sessions.find(Packet->m_SessionID);
		if (Iterator != m_Sessions.cend()) {
			if (m_DataBase->TryJoinGame(Packet)) {
				Packet->m_PacketType = EPACKETTYPE::EPT_PLAYER;
				Packet->m_MessageType = EPACKETMESSAGETYPE::EPMT_NEWPLAYER;
				Packet->m_FailedReason = EPACKETFAILEDTYPE::EPFT_SUCCEED;
				Packet->m_Socket = Info->m_Socket;
				Packet->m_UniqueKey = (++std::get<ESI_UNIQUEKEY>(Iterator->second));
				Packet->m_RankInformation.m_CurrentRank = std::get<ESI_PLAYERINFORMATION>(Iterator->second).size() + 1;
				Packet->m_bIsLeader = false;

				BroadCast(Packet, std::get<ESI_PLAYERINFORMATION>(Iterator->second));
				BroadCast(std::get<ESI_PLAYERINFORMATION>(Iterator->second), Packet);

				std::get<ESI_PLAYERINFORMATION>(Iterator->second).push_back(*Packet);

				Packet->m_MessageType = EPACKETMESSAGETYPE::EPMT_JOIN;
				m_Server->Send(Info, Packet);
				std::cout << "Join Session Is Succeed! - " << Info->m_Socket << '\t' << Packet->m_PlayerNickName << '\t' << Packet->m_RankInformation.m_CurrentRank << std::endl;
				return;
			}
			Packet->m_PacketType = EPACKETTYPE::EPT_PLAYER;
			Packet->m_MessageType = EPACKETMESSAGETYPE::EPMT_JOIN;
			Packet->m_FailedReason = EPACKETFAILEDTYPE::EPFT_FAILED;
			m_Server->Send(Info, Packet);
			std::cout << "Join Session Is Failure! - " << Info->m_Socket << '\t' << Packet->m_PlayerNickName << std::endl;
		}
		else {
			if (m_DataBase->TryJoinGame(Packet)) {
				Packet->m_PacketType = EPACKETTYPE::EPT_PLAYER;
				Packet->m_MessageType = EPACKETMESSAGETYPE::EPMT_JOIN;
				Packet->m_FailedReason = EPACKETFAILEDTYPE::EPFT_SUCCEED;
				Packet->m_Socket = Info->m_Socket;
				Packet->m_UniqueKey = 0;
				Packet->m_RankInformation.m_CurrentRank = 1;
				Packet->m_bIsLeader = true;

				std::vector<GAMEPACKET> PlayerList;
				PlayerList.push_back(*Packet);
				m_Sessions.insert(std::make_pair(Packet->m_SessionID, std::make_tuple(0, PlayerList)));

				m_Server->Send(Info, Packet);
				std::cout << "Join Session Is Succeed! - " << Info->m_Socket << '\t' << Packet->m_PlayerNickName << '\t' << Packet->m_RankInformation.m_CurrentRank << std::endl;
				return;
			}
			Packet->m_PacketType = EPACKETTYPE::EPT_PLAYER;
			Packet->m_MessageType = EPACKETMESSAGETYPE::EPMT_JOIN;
			Packet->m_FailedReason = EPACKETFAILEDTYPE::EPFT_FAILED;
			m_Server->Send(Info, Packet);
			std::cout << "Join Session Is Failure! - " << Info->m_Socket << '\t' << Packet->m_PlayerNickName << std::endl;
		}
	}
}

void PacketProcessor::UpdatePlayerInformation(SOCKETINFO* Info, GAMEPACKET*& Packet) {
	if (Packet && Info) {
		auto Session = m_Sessions.find(Packet->m_SessionID);
		if (Session != m_Sessions.cend()) {
			auto Character = std::find_if(std::get<ESI_PLAYERINFORMATION>(Session->second).begin(), std::get<ESI_PLAYERINFORMATION>(Session->second).end(), [&](const GAMEPACKET& Data) -> bool { if (Data.m_UniqueKey == Packet->m_UniqueKey) { return true; } return false; });
			if (Character != std::get<ESI_PLAYERINFORMATION>(Session->second).cend()) {
				(*Character) = *Packet;
				Packet->m_PacketType = EPACKETTYPE::EPT_PLAYER;
				Packet->m_MessageType = EPACKETMESSAGETYPE::EPMT_UPDATE;
				Packet->m_FailedReason = EPACKETFAILEDTYPE::EPFT_SUCCEED;
				Packet->m_RankInformation.m_CurrentRank = UpdatePlayerRanking(std::get<ESI_PLAYERINFORMATION>(Session->second), Character);
				
				BroadCast(Packet, std::get<ESI_PLAYERINFORMATION>(Session->second));
			}
		}
	}
}

void PacketProcessor::DisconnectPlayer(SOCKETINFO* Info, GAMEPACKET*& Packet) {
	if (Packet && Info && m_DataBase) {
		if (m_DataBase->TryDisconnectGame(Packet->m_SessionID, m_Sessions)) {
			auto Iterator = m_Sessions.find(Packet->m_SessionID);
			if (Iterator != m_Sessions.cend()) {
				auto Character = std::find_if(std::get<ESI_PLAYERINFORMATION>(Iterator->second).begin(), std::get<ESI_PLAYERINFORMATION>(Iterator->second).end(), [&](const GAMEPACKET& Data) -> bool { if (Data.m_UniqueKey == Packet->m_UniqueKey) { return true; } return false; });
				if (Character != std::get<ESI_PLAYERINFORMATION>(Iterator->second).cend()) {
					std::get<ESI_PLAYERINFORMATION>(Iterator->second).erase(Character);
					Packet->m_PacketType = EPACKETTYPE::EPT_PLAYER;
					Packet->m_MessageType = EPACKETMESSAGETYPE::EPMT_DISCONNECTOTHER;
					Packet->m_FailedReason = EPACKETFAILEDTYPE::EPFT_SUCCEED;

					ResetPlayerRanking(std::get<ESI_PLAYERINFORMATION>(Iterator->second), Packet);

					BroadCast(Packet, std::get<ESI_PLAYERINFORMATION>(Iterator->second));
					std::cout << "Disconnect Session Is Succeed! - " << Info->m_Socket << '\t' << Packet->m_SessionID << '\t' << Packet->m_UniqueKey << std::endl;
					return;
				}
			}
		}
	}
}

void PacketProcessor::StartGame(SOCKETINFO* Info, GAMEPACKET*& Packet) {
	if (Packet && Info && m_DataBase) {
		auto Iterator = m_Sessions.find(Packet->m_SessionID);
		if (Iterator != m_Sessions.cend()) {
			Packet->m_PacketType = EPACKETTYPE::EPT_PLAYER;
			Packet->m_MessageType = EPACKETMESSAGETYPE::EPMT_STARTGAME;
			Packet->m_FailedReason = EPACKETFAILEDTYPE::EPFT_SUCCEED;

			BroadCast(Packet, std::get<ESI_PLAYERINFORMATION>(Iterator->second));

			std::cout << "Start Game Succeed! - " << Info->m_Socket << std::endl;
			return;
		}
		Packet->m_PacketType = EPACKETTYPE::EPT_PLAYER;
		Packet->m_MessageType = EPACKETMESSAGETYPE::EPMT_STARTGAME;
		Packet->m_FailedReason = EPACKETFAILEDTYPE::EPFT_FAILED;
		m_Server->Send(Info, Packet);
		std::cout << "Start Game Failure! - " << Info->m_Socket << std::endl;
	}
}

void PacketProcessor::ChangeReadyState(SOCKETINFO* Info, GAMEPACKET *& Packet) {
	if (Info && Packet) {
		auto Iterator = m_Sessions.find(Packet->m_SessionID);
		if (Iterator != m_Sessions.cend()) {
			Packet->m_PacketType = EPACKETTYPE::EPT_PLAYER;
			Packet->m_MessageType = EPACKETMESSAGETYPE::EPMT_READY;
			Packet->m_FailedReason = EPACKETFAILEDTYPE::EPFT_SUCCEED;

			BroadCast(Packet, std::get<ESI_PLAYERINFORMATION>(Iterator->second));

			std::cout << "Change Ready State Succeed! - " << Info->m_Socket << std::endl;
			return;
		}
		std::cout << "Change Ready State Failure - " << Info->m_Socket << std::endl;
	}
}

void PacketProcessor::AddNewSpawnTimer(SOCKETINFO* Info, GAMEPACKET*& Packet) {
	if (Packet && Info) {
		SPAWNERPACKET* SendPacket = new SPAWNERPACKET;
		SendPacket->m_PacketType = Packet->m_PacketType;
		SendPacket->m_MessageType = Packet->m_MessageType;
		SendPacket->m_FailedReason = EPACKETFAILEDTYPE::EPFT_SUCCEED;
		SendPacket->m_ItemInformation = Packet->m_ItemInformation;
		SendPacket->m_ItemInformation.m_ItemType = static_cast<EITEMTYPE>(std::uniform_int_distribution<int>(static_cast<unsigned char>(EITEMTYPE::EIT_HEAL), static_cast<unsigned char>(EITEMTYPE::EIT_COUNT) - 1)(m_RandomAlgorithm));
		
		std::unique_lock<std::mutex> Lock(m_Lock);
		m_TimerList.push_back(std::make_tuple(std::chrono::system_clock::now(), std::chrono::duration<float>(ItemRespawnTime), Packet->m_SessionID, SendPacket));
	}
}

void PacketProcessor::BroadCast(PACKET* Packet, const std::vector<struct GAMEPACKET>& PlayerList) {
	SOCKETINFO* Info = new SOCKETINFO;
	memset(Info, 0, sizeof(SOCKETINFO));

	for (auto It : PlayerList) {
		Info->m_Socket = It.m_Socket;

		m_Server->Send(Info, Packet);
	}
}

void PacketProcessor::BroadCast(const std::vector<struct GAMEPACKET>& PlayerList, GAMEPACKET*& Packet) {
	SOCKETINFO* Info = new SOCKETINFO;
	memset(Info, 0, sizeof(SOCKETINFO));
	Info->m_Socket = Packet->m_Socket;

	for (auto It : PlayerList) {
		It.m_PacketType = Packet->m_PacketType;
		It.m_MessageType = Packet->m_MessageType;
		It.m_FailedReason = Packet->m_FailedReason;

		m_Server->Send(Info, &It);
	}
}

int PacketProcessor::UpdatePlayerRanking(std::vector<struct GAMEPACKET>& CharacterLists, std::vector<struct GAMEPACKET>::iterator& Player) {
	for (auto& It : CharacterLists) {
		if (It.m_UniqueKey != (*Player).m_UniqueKey) {
			if ((*Player).m_RankInformation.m_CurrentRank > It.m_RankInformation.m_CurrentRank) {
				if ((*Player).m_RankInformation.m_CurrentLab > It.m_RankInformation.m_CurrentLab) {
					SwapRanking(It.m_RankInformation.m_CurrentRank, (*Player).m_RankInformation.m_CurrentRank);
				}
				else if ((*Player).m_RankInformation.m_CurrentLab == It.m_RankInformation.m_CurrentLab) {
					if ((*Player).m_RankInformation.m_CurrentSplinePoint > It.m_RankInformation.m_CurrentSplinePoint) {
						SwapRanking(It.m_RankInformation.m_CurrentRank, (*Player).m_RankInformation.m_CurrentRank);
					}
					else if ((*Player).m_RankInformation.m_CurrentSplinePoint == It.m_RankInformation.m_CurrentSplinePoint) {
						if ((*Player).m_RankInformation.m_SplinePointDistance < It.m_RankInformation.m_SplinePointDistance) {
							SwapRanking(It.m_RankInformation.m_CurrentRank, (*Player).m_RankInformation.m_CurrentRank);
						}
					}
				}
			}
		}
	}
	return (*Player).m_RankInformation.m_CurrentRank;
}

void PacketProcessor::ResetPlayerRanking(std::vector<struct GAMEPACKET>& CharacterList, const GAMEPACKET* Packet) {
	for (auto It : CharacterList) {
		if (It.m_UniqueKey != Packet->m_UniqueKey) {
			if (It.m_RankInformation.m_CurrentRank > Packet->m_RankInformation.m_CurrentRank) {
				It.m_RankInformation.m_CurrentRank -= 1;
			}
		}
	}
}