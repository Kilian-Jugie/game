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

	protected:
		int safeput(string_t, bool) const;
		

		public:
		void init() const;

		LoggerParent(const string_t& file, const string_t& prefix, const string_t& division) : m_File(file), m_Prefix(prefix), m_Division(division), lastEndl(true) {}

		virtual inline const LoggerParent& operator<<(const string_t& v) const {
			safeput(v, true);
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

	class LogInFileInfo : public LoggerParent {

	public:
		LogInFileInfo(string_t file, string_t div="") : LoggerParent(file, "INFO", div) {}

		const LoggerParent& operator<<(const string_t& v) const final override {
			safeput(v, false);
			return *this;
		}
	};
}