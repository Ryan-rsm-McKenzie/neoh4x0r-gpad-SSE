#pragma once

#include "common/ICriticalSection.h"
#define _STR(x) #x
#define STR(x) _STR(x)
#define _E(x) __pragma(message("ERROR: error : "_STR(x) " :: " __FILE__ "@"STR(__LINE__)))
#define _W(x) __pragma(message("WARN: warning : "_STR(x) " :: " __FILE__ "@"STR(__LINE__)))
#define _NI(x) __pragma(message("TODO: warning : "_STR(x) " :: " __FILE__ "@"STR(__LINE__)))

class Log
{
public:
	static void Console(const char * fmt, ...);
	static void Info(const char * fmt, ...);
	static void Warn(const char * fmt, ...);
	static void Error(const char * fmt, ...);
	static void Debug(const char * fmt, ...);
	static void VerboseDebug(const char * fmt, ...);
private:
	static ICriticalSection	m_logLock;
};
