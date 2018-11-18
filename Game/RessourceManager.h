#pragma once
#include "RessourceFile.h"

namespace UL {
	class RessourceManager {
		string_t m_Folder;
		bool m_Reccurent;
	public:
		RessourceManager(string_t baseFolder, bool recurrent=true) : m_Folder(baseFolder), m_Reccurent(recurrent) {}


	};
}