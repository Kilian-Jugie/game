#pragma once
#include "Error.h"
#include <filesystem>

namespace UL {
	class RessourceFile {
		string_t m_Folder, m_FullFilename;
		int m_Type;

	public:
		RessourceFile(string_t folder, string_t filename, int type) : m_Folder(folder), m_FullFilename(filename), m_Type(type) {}

		const string_t& getFolder() const {
			return m_Folder;
		}

		const string_t& getFilename() const {
			return m_FullFilename;
		}

		int getType() const {
			return m_Type;
		}
	};
}