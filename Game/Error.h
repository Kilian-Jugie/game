#pragma once
#include "logger.h"
#include "cnl.h"

namespace UL {
	class ThrowableEvent {
	protected:
		virtual void print(string_t msg) {
			getLogger() << msg;
		}

	public:
		virtual LoggerParent getLogger() = 0;

		ThrowableEvent() {}

		ThrowableEvent& operator<<(string_t t) { print(t); return *this; }
		ThrowableEvent& operator<<(int i) { print(std::to_string(i)); return *this; }
		ThrowableEvent& operator<<(const RessourceFile& f);
	};

	class Error : public ThrowableEvent {
	public:
		LoggerParent getLogger() override final {
			return LogError{ "global.log", "" };
		}
	};


	class Warning : public ThrowableEvent {
	public:
		LoggerParent getLogger() override final {
			return LogWarning{ "global.log", "" };
		}
	};

	class Info : public ThrowableEvent {
	public:
		LoggerParent getLogger() override final {
			return LogInfo{ "global.log", "" };
		}
	};


	class Info2File : public ThrowableEvent {
	public:
		LoggerParent getLogger() override final {
			return LogInfo{ "global.log", "" };
		}
	};


}