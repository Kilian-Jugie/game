#pragma once
#include "Defines.h"
#include "RessourceFile.h"
#include <filesystem>
#include <stdarg.h>

namespace UL {

	class RessourceManager {
	public:
		enum TYPE {
			ERRORED,
			TEXTURE,
			MODEL,
			OTHER
		};

		RessourceManager(string_t baseFolder) : m_BaseFolder(baseFolder) {

		}

		void addCorrespondance(TYPE t, string_t folder) {
			m_CorrespondingTable.push_back({ t,folder });
		}

		RessourceFile getRessource(TYPE t, string_t name) {
			string_t folder;
			for (auto i : m_CorrespondingTable) {
				if (i.first == t)
					folder = i.second;
			}
			if (!hasExtension(name)) {
				
				for (std::filesystem::directory_iterator it(std::filesystem::absolute(folder).generic_string()); it != std::filesystem::end(it); ++it) {
					if (name == it->path().stem()) {
						name = it->path().generic_string();
						break;
					}
				}
				if (!hasExtension(name)) {
					Error{} << "Ressource file '" << name << "' does not exists in the type corresponding folder (" << std::filesystem::absolute(folder).generic_string() << ")\n";
					return RessourceFile(TXT(""), TXT(""), ERRORED);
				}
			}
			return std::move(getTypedRessource(folder, name, t));
		}

	private:

		RessourceFile getTypedRessource(string_t folder, string_t file, TYPE t) {
			return std::move(RessourceFile(folder, file, t));
		}

		bool hasExtension(string_t name) {
			for (string_t::const_iterator it(name.end() - 1), beg(name.begin()); it > beg; --it)
				if (*it == TXT('.'))
					return true;
			return false;
		}

		string_t m_BaseFolder;
		vector_t<pair_t<TYPE, string_t>> m_CorrespondingTable;
	};
}