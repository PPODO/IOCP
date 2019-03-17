#pragma once
#pragma comment(lib, "libmysql.lib")
#include <mysql.h>

#ifdef HANSEIDLL_EXPORTS
#define HANSEIDLL_API __declspec(dllexport)
#else
#define HANSEIDLL_API __declspec(dllimport)
#endif

#include <functional>
#include <iostream>
#include <sstream>
#include <vector>
#include <string>

namespace DATABASE {
	class HANSEIDLL_API DataBaseConnectorClass {
	private:
		MYSQL m_LoginConnector;
		MYSQL m_SessionConnector;

	protected:
		MYSQL* m_LoginHandle;
		MYSQL* m_SessionHandle;

	public:
		DataBaseConnectorClass();
		virtual ~DataBaseConnectorClass();

	};
}

namespace PACKET {
	enum HANSEIDLL_API EFAILED {
		EF_FAILED,
		EF_EXIST,
		EF_SUCCEED
	};

	enum HANSEIDLL_API ELOGINFAILED {
		ELF_WRONGID,
		ELF_WRONGPASS,
		ELF_FAILED,
		ELF_SUCCEED
	};

	enum HANSEIDLL_API ESIGNUPFAILED {
		ESF_EXISTNICKNAME,
		ESF_EXISTID,
		ESF_FAILED,
		ESF_SUCCEED
	};

	enum HANSEIDLL_API EJOINFAILED {
		EJF_FAILED,
		EJF_INVALIDSESSION,
		EJF_WRONGPASS,
		EJF_MAXPLAYER,
		EJF_SUCCEED
	};

	enum HANSEIDLL_API ENEWSESSIONFAILED {
		ENSF_FAILED,
		ENSF_EXIST,
		ENSF_SUCCEED
	};
}

namespace PLAYER {
	struct Vector {
		float X, Y, Z;

		Vector() { X = 0, Y = 0, Z = 0; };

		friend std::ostream& operator<<(std::ostream& os, Vector& Vec) {
			os << Vec.X << std::endl;
			os << Vec.Y << std::endl;
			os << Vec.Z << std::endl;

			return os;
		}

		friend std::istream& operator>>(std::istream& is, Vector& Vec) {
			is >> Vec.X;
			is >> Vec.Y;
			is >> Vec.Z;

			return is;
		}
	};

	class HANSEIDLL_API Character {
	public:
		unsigned int m_UniqueKey;
		std::string m_PlayerName;
		Vector m_Location;
		Vector m_Rotation;

		SOCKET m_Socket;

	public:
		Character();
		~Character();

		friend std::ostream& operator<<(std::ostream& os, Character& Info) {
			os << Info.m_UniqueKey << std::endl;
			os << Info.m_PlayerName << std::endl;
			os << Info.m_Location << std::endl;
			os << Info.m_Rotation << std::endl;

			return os;
		}

		friend std::istream& operator>>(std::istream& is, Character& Info) {
			is >> Info.m_UniqueKey;
			is >> Info.m_PlayerName;
			is >> Info.m_Location;
			is >> Info.m_Rotation;

			return is;
		}

		void operator=(const Character& Char) {
			this->m_Location = Char.m_Location;
			this->m_PlayerName = Char.m_PlayerName;
			this->m_Rotation = Char.m_Rotation;
			this->m_UniqueKey = Char.m_UniqueKey;
		}
	};

	class HANSEIDLL_API CharacterInformation {
	public:
		std::vector<Character> m_Characters;

	public:
		CharacterInformation();
		~CharacterInformation();

		friend std::ostream& operator<<(std::ostream& os, CharacterInformation& Info) {
			for (auto Iterator : Info.m_Characters) {
				os << Iterator << std::endl;
			}
			return os;
		}

		friend std::istream& operator>>(std::istream& is, CharacterInformation& Info) {
			for (auto& Iterator : Info.m_Characters) {
				is >> Iterator;
			}
			return is;
		}
	};
}

namespace SESSION {
	enum GAMESTATE { LOBBY, INGAME };

	class HANSEIDLL_API Session {
	public:
		std::string m_SessionName;
		int m_MaxPlayer;
		bool m_bUsePassword;
		std::string m_Password;
		int m_CurrentPlayer;
		int m_GameState;

	public:
		Session();
		~Session();

		friend std::ostream& operator<<(std::ostream& os, Session& Info) {
			os << Info.m_SessionName << std::endl;
			os << Info.m_MaxPlayer << std::endl;
			os << Info.m_bUsePassword << std::endl;
			os << Info.m_Password << std::endl;
			os << Info.m_CurrentPlayer << std::endl;
			os << Info.m_GameState << std::endl;

			return os;
		}

		friend std::istream& operator>>(std::istream& is, Session& Info) {
			is >> Info.m_SessionName;
			is >> Info.m_MaxPlayer;
			is >> Info.m_bUsePassword;
			is >> Info.m_Password;
			is >> Info.m_CurrentPlayer;
			is >> Info.m_GameState;

			return is;
		}

	};

	class HANSEIDLL_API SessionInformation {
	public:
		std::vector<Session> m_Sessions;

	public:
		SessionInformation();
		~SessionInformation();

		friend std::ostream& operator<<(std::ostream& os, SessionInformation& Info) {
			for (auto Iterator : Info.m_Sessions) {
				os << Iterator;
			}
			return os;
		}

		friend std::istream& operator>>(std::istream& is, SessionInformation& Info) {
			for (auto Iterator : Info.m_Sessions) {
				is >> Iterator;
			}
			return is;
		}
	};
}