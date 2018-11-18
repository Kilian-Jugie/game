#pragma once
#include "Defines.h"
#include <string>

namespace OC {
	template<typename _ch_t>class ocString {
		using ocStr_t = std::basic_string<_ch_t>;
		
		 ocStr_t m_String;

	public:


		inline bool empty() const {
			return m_String.empty();
		}

		inline size_t size() const {
			return m_String.size();
		}

		inline _ch_t& operator[](const size_t& i) {
			return m_String[i];
		}

		inline _ch_t& at(const size_t& i) {
			return this->operator[](i);
		}

		inline const std::string& toStr() const {
			return toCustomStr<std::string>();
		}

		inline const std::wstring& toWStr() const {
			return toCustomStr<std::wstring>();
		}

		inline ocString<_ch_t>& operator=(const ocString<_ch_t>& c) {
			m_String = c.get();
			return *this;
		}


		inline ocString<_ch_t>& operator=(const ocStr_t& c) {
			m_String = c;
			return *this;
		}

		inline ocString<_ch_t>& operator=(const _ch_t* c) {
			m_String = c;
			return *this;
		}

		inline const ocStr_t& get() const {
			return m_String;
		}

		inline ocStr_t getCopy() const {
			return m_String;
		}

		template<typename _Ty>inline const _Ty& toCustomStr() const {
			return _Ty(m_String.begin(), m_String.end());
		}

		ocString& replace(ocString, ocString);
		ocString& replace(ocStr_t base, ocStr_t by) {
			
			m_String.replace(m_String.find(base), m_String.find(base) + base.size(), by);
			return *this;
		}

	
		
	};
}