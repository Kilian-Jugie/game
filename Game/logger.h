#pragma once
#include <string>
#include <fstream>
#include <iostream>
#include <chrono>
#include <ctime>
#include <sstream>
#include "Defines.h"

namespace UL {
	
	class LoggerParent {
		string_t m_Prefix, m_File, m_Division;
		bool lastEndl;

		int safeput(const string_t&);

		public:
		LoggerParent(string_t file, string_t prefix, string_t division) : m_File(file), m_Prefix(prefix), m_Division(division), lastEndl(true) {}

		inline LoggerParent& operator<<(const string_t& v) {
			safeput(v);
			return *this;
		}
	};

	class LogError : public LoggerParent {

	public:
		LogError(string_t file, string_t div="") : LoggerParent(file, "ERROR", div) {}
	};

	class LogWarning : public LoggerParent {

	public:
		LogWarning(string_t file, string_t div = "") : LoggerParent(file, "WARNING", div) {}
	};

	class LogInfo : public LoggerParent {

	public:
		LogInfo(string_t file, string_t div = "") : LoggerParent(file, "INFO", div) {}

	};
}