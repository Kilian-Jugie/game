#pragma once
#include "Defines.h"

namespace UL {
	class Tools {
	public:
		static string_t& insertAfterAll(string_t& _where, char afterWhat, const string_t& what) {
			string_t::iterator it(_where.begin());
			for (; it != _where.end(); ++it) {
				if ((*it) == afterWhat) {
					if (it == (_where.end() - 1))
						_where.resize(_where.size() + 1);
					it = _where.insert(it+1, what.begin(), what.end());
					_where.erase(_where.end() - 1);
					it += what.size()-1;
				}
			}
			return _where;
		}
	};
}
