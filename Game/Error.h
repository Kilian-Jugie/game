#pragma once
#include "logger.h"

namespace UL {
	class ThrowableEvent {
	protected:
		virtual void print(string_t msg) = 0;

	public:
		ThrowableEvent() {}

		ThrowableEvent& operator<<(string_t t) { print(t); return *this; }
	};

	class Error : public ThrowableEvent {
		void print(string_t msg) override final {

			LogError{ "global.log", "" } << msg;
		}

	public:
	};


	class Warning : public ThrowableEvent {
		void print(string_t msg) override final {
			LogWarning{ "global.log", "" } << msg;
		}

	public:
	};

	class Info : public ThrowableEvent {
		void print(string_t msg) override final {
			LogInfo{ "global.log", "" } << msg;
		}
		
	public:
	};


}