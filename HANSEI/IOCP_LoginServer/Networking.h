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
		EF_EXIST,
		EF_FAILED,
		EF_SUCCEED
	};

	enum HANSEIDLL_API EJOINFAILED {
		EJF_FAILED,
		EJF_INVALIDSESSION,
		EJF_WRONGPASS,
		EJF_MAXPLAYER,
		EJF_SUCCEED
	};
}

namespace PLAYER {
	class HANSEIDLL_API Character {
	public:


	public:
		Character();
		~Character();

		friend std::ostream& operator<<(std::ostream& os, Character& Info) {

			return os;
		}

		friend std::istream& operator>>(std::istream& is, Character& Info) {

			return is;
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
			int Size = -1;
			is >> Size;

			for (int i = 0; i < Size; i++) {
				Character NewCharacter;
				is >> NewCharacter;

				Info.m_Characters.push_back(NewCharacter);
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