#include "Logger.h"
#include "gpadconf.h"
#include "skse64/GameAPI.h"

ICriticalSection Log::m_logLock;

void Log::Console(const char * fmt, ...)
{
	if(!gpadconf::VerboseDebugEnabled()) { return; }
	IScopedCriticalSection lock(&m_logLock);
	va_list	args;
	va_start(args, fmt);
	ConsoleManager	* mgr = *g_console;
	if(mgr)
	{
		CALL_MEMBER_FN(mgr, VPrint)(fmt, args);
	}
	//Log::VerboseDebug(fmt, args);
	gLog.Log(IDebugLog::kLevel_DebugMessage, fmt, args);
	va_end(args);
}
void Log::Info(const char * fmt, ...)
{
	IScopedCriticalSection lock(&m_logLock);
	va_list args;
	va_start(args, fmt);
	gLog.Log(IDebugLog::kLevel_Message, fmt, args);
	va_end(args);
}
void Log::Warn(const char * fmt, ...)
{
	IScopedCriticalSection lock(&m_logLock);
	va_list args;
	va_start(args, fmt);
	gLog.Log(IDebugLog::kLevel_Warning, fmt, args);
	va_end(args);
}
void Log::Error(const char * fmt, ...)
{
	IScopedCriticalSection lock(&m_logLock);
	va_list args;
	va_start(args, fmt);
	gLog.Log(IDebugLog::kLevel_Error, fmt, args);
	va_end(args);
}
void Log::Debug(const char * fmt, ...)
{
	if(!gpadconf::DebugEnabled()) { return; }
	IScopedCriticalSection lock(&m_logLock);
	va_list args;
	va_start(args, fmt);
	gLog.Log(IDebugLog::kLevel_DebugMessage, fmt, args);
	va_end(args);
}
void Log::VerboseDebug(const char * fmt, ...)
{
	if(!gpadconf::VerboseDebugEnabled()) { return; }
	IScopedCriticalSection lock(&m_logLock);
	va_list args;
	va_start(args, fmt);
	gLog.Log(IDebugLog::kLevel_DebugMessage, fmt, args);
	va_end(args);
}
