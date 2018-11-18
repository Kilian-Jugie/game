#pragma once
#define KTOOLS_STR_NO_ALIAS
#include "ktools.h"

#define EXPAND(x) x
#define CONCAT(x, ...) x##__VA_ARGS__ 

#ifndef UNICODE
using string_t = std::string;
using char_t = char;
#define uto_string UL::Tools::to_string
#define TXT(x) x
#else
using string_t = std::wstring;
using char_t = wchar_t;
#define uto_string UL::Tools::to_wstring
#defie TXT(x) L##x
#endif 

using fstring_t = std::string;

using ofstream_t = std::basic_ofstream<char_t>;
using ifstream_t = std::basic_ifstream<char_t>;

