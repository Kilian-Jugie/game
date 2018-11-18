#pragma once
#include "Error.h"


namespace UL {
	class RessourceFile {
		fstring_t m_FileName, m_FullPath, m_Extension;

	public:
		enum RF_ERROR {
			NO_ERROR,
			FNAME_EMPTY,
			NAME_TOO_SHORT,
			FPATH_NEED_EXTENSION
		};

		enum RF_TYPE {
			MODEL,
			TEXTURE,
			OTHER
		};

		RessourceFile(/*fstring_t file*/) {}

		int parseFileName(fstring_t f) {
			if (f.empty()) {
				Error{} << "Filename of a ressource file is empty !\n";
				return FNAME_EMPTY;
			}
			if (f.size() < 2) {
				Error{} << "Filename '" << f << "' is too short. Minimum size: 2\n";
				return NAME_TOO_SHORT;
			 }
			if (f[1] == ':') {
				for (unsigned int i(0u); i < f.size(); ++i) {
					if (f[i] == '/')
						f[i] = '\\';
				}
				int i(f.size());
				while (f[i] != '\\'&&f[i]!='.') {
					--i;
				}
				if (f[i] != '.') {
					Error{} << "Filename '" << f << "' is detected as full path so a extension is needed !\n";
					return FPATH_NEED_EXTENSION;
				}
				
				fstring_t extesion(f.substr(i));
				Info{} << "File ext: " << extesion << "\n";
			}
		}
	};
}