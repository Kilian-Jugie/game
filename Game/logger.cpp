#include "logger.h"

namespace UL {

	int UL::LoggerParent::safeput(const string_t &t) {
		string_t toDisp(t), time;
		toDisp.reserve(12 + m_Prefix.size() + t.size()); //Minimum possible size
		ofstream_t of(m_File);
		if (!of.is_open()) {
			std::cerr << "ERROR: cannot open log file '" << m_File << "'\n";
			return 1;
		}
		if (lastEndl) {
			std::time_t t(std::time(0));
			std::tm* n(std::localtime(&t));
			time = "[" + std::to_string(n->tm_hour) + ":" + std::to_string(n->tm_min) +":" + std::to_string(n->tm_sec) + "]";
			time += {"[" + m_Prefix + "] "};
			lastEndl = false;
		}
		if (toDisp[toDisp.size() - 1] == '\n') lastEndl = true;
		//Tools::replaceAll(toDisp, '\n', time);
		
		//toDisp.replace(rep.begin(), rep.end(), time.c_str());
		std::cout << t;
		of << t;

		of.close();
		return 0;
	}

	/*const string_t ocLogger::getLogPrefix() {
		switch (m_Type) {
		case ocLoggerUtils::LOG_ERROR: {
			return TXT("ERROR");
		} break;
		case ocLoggerUtils::LOG_INFO: {
			return TXT("INFO");
		} break;
		case ocLoggerUtils::LOG_WARNING: {
			return TXT("WARNING");
		} break;
		case ocLoggerUtils::LOG_DEBUG: {
			return TXT("DEBUG");
		} break;
		default: __assume(0);
		}
		__assume(0);
	}

	const string_t ocLogger::getTime() {
		std::time_t t(std::time(0));
		std::tm* n(std::localtime(&t));
		return to_string(n->tm_hour) + TXT(":") + to_string(n->tm_min) + TXT(":") + to_string(n->tm_sec);
	}

	ocLogger& ocLogger::put(const string_t& w) {
		if (w.size() == 0) return *this;
		string_t b;
		if (m_lastEndl) {
			b += (TXT("[") + getTime() + TXT("] [") + getLogPrefix() + TXT("]: "));
			m_lastEndl = false;
		}
		b += w;
		if (b[b.size() - 1] == TXT('\0')) b.erase(b.end() - 1);
		m_Out << b;
		std::cout << b; //A changer !
		if (b[b.size() - 1] == TXT('\n')) m_lastEndl = true;
		return *this;
	}

	void ocLogger::refreshStream(bool truncFile = true) {
		if (m_Out.is_open())
			m_Out.close();
		m_Out.open(m_Str, std::ios_base::binary | (truncFile ? std::ios_base::trunc : std::ios_base::app));
	}

	void ocLogger::closeStream() {
		m_Out.close();
	}

	ocLogger::ocLogger(std::string filename, ocLoggerUtils::LOG_TYPE t) : m_Out(filename.c_str(), std::ios::binary | std::ios::trunc), m_Type(t), m_Str(filename) {
		if (!m_Out.is_open()) {
			std::cerr << TXT("Critical error: cannot open ") << filename.c_str() << TXT(" for logs output !\n");
			exit(1);
		}
	}

	ocLogger::ocLogger(const ocLogger& n) : m_Out(n.m_Str.c_str(), std::ios::binary | std::ios::trunc), m_Type(n.m_Type), m_Str(n.m_Str) {}

	ocLogger::~ocLogger() {
		closeStream();
	}

	ocLogger ocLogger::createLoggerCopy() {
		return std::move(ocLogger(m_Str, m_Type));
	}

	void ocLogger::changeFileOutput(string_t newFile, bool truncFile = true) {
		m_Str = newFile;
		refreshStream(truncFile);
	}

	template<typename T>ocLogger& ocLogger::operator<<(const T& w) {
		return this->put(to_string(w));
	}*/
}