#pragma once
#include <intrin.h>
#include <stdio.h>
#include <interface/slog-i.h>

#ifndef E_RANGE
#define E_RANGE 9944
#endif

#ifndef OUTLOG_LEVEL
#define OUTLOG_LEVEL 0	//LOG_LEVEL_TRACE
#endif

//! check VC VERSION. DO NOT TOUCH
//! format micro cannot support VC6 or VS2003, please use stream input log, like LOGI, LOGD, LOG_DEBUG, LOG_STREAM ...
#if _MSC_VER >= 1400 //MSVC >= VS2005
#define LOG4Z_FORMAT_INPUT_ENABLE
#endif

#if !defined(WIN32)  && !defined(_WIN64)
#define LOG4Z_FORMAT_INPUT_ENABLE
#endif

namespace SOUI
{
	class Log4zBinary;
	class Log4zStream;

	class SOUI_EXP SLogHelper
	{
	public:
		static void LogFormat(int id,int level,const char* filter, int line , const char * func , const char * file,const void *pRetAddr, const wchar_t *logformat, ...);
		static void LogFormat(int id,int level,const char* filter, int line , const char * func , const char * file,const void *pRetAddr, const char *logformat, ...);
		static void LogStream(int id,int level,const char* filter, int line , const char * func , const char * file,const void *pRetAddr, Log4zStream & stream);
		static void LogFormat(const char* name,int level,const char* filter, int line , const char * func , const char * file,const void *pRetAddr, const wchar_t *logformat, ...);
		static void LogFormat(const char* name,int level,const char* filter, int line , const char * func , const char * file,const void *pRetAddr, const char *logformat, ...);
		static void LogStream(const char* name,int level,const char* filter, int line , const char * func , const char * file,const void *pRetAddr, Log4zStream & stream);
		static void SetLogMgr(ILog4zManager *pLogMgr);
	private:
		static void _PushLog(ILog4zManager * pLogMgr,const char *logbuf,int id,int level,const char* filter, int line , const char * func , const char * file,const void *pRetAddr);
		static void _LogFormat(int id,int level,const char* filter, int line , const char * func , const char * file,const void *pRetAddr, const char *logformat, va_list & args);
		static void _LogFormat(int id,int level,const char* filter, int line , const char * func , const char * file,const void *pRetAddr, const wchar_t *logformat, va_list & args);
		static ILog4zManager * GetLogMgr();
		static ILog4zManager * SLogHelper::s_LogMgr;
	};
}


//! base micro.
#define SOUI_LOG_STREAM(id_or_name, filter, level,  log) \
    do{\
		char *logBuf= (char*)malloc(SOUI::LOG4Z_LOG_BUF_SIZE+1);\
		SOUI::Log4zStream ss(logBuf, SOUI::LOG4Z_LOG_BUF_SIZE);\
		ss << log;\
		SLogHelper::LogStream(id_or_name,level,filter,__LINE__,__FUNCTION__,__FILE__,_ReturnAddress(),ss);\
		free(logBuf);\
    } while (0)

//! fast micro
#define LOG_TRACE(id, filter, log) SOUI_LOG_STREAM(id, filter, SOUI::ILog4zManager::LOG_LEVEL_TRACE, log)
#define LOG_DEBUG(id, filter, log) SOUI_LOG_STREAM(id, filter, SOUI::ILog4zManager::LOG_LEVEL_DEBUG, log)
#define LOG_INFO(id, filter, log)  SOUI_LOG_STREAM(id, filter, SOUI::ILog4zManager::LOG_LEVEL_INFO, log)
#define LOG_WARN(id, filter, log)  SOUI_LOG_STREAM(id, filter, SOUI::ILog4zManager::LOG_LEVEL_WARN, log)
#define LOG_ERROR(id, filter, log) SOUI_LOG_STREAM(id, filter, SOUI::ILog4zManager::LOG_LEVEL_ERROR, log)
#define LOG_ALARM(id, filter, log) SOUI_LOG_STREAM(id, filter, SOUI::ILog4zManager::LOG_LEVEL_ALARM, log)
#define LOG_FATAL(id, filter, log) SOUI_LOG_STREAM(id, filter, SOUI::ILog4zManager::LOG_LEVEL_FATAL, log)

//! super micro.
#define LOGT(filter, log ) LOG_TRACE(SOUI::LOG4Z_MAIN_LOGGER_ID,filter, log )
#define LOGD(filter, log ) LOG_DEBUG(SOUI::LOG4Z_MAIN_LOGGER_ID, filter,log )
#define LOGI(filter, log ) LOG_INFO(SOUI::LOG4Z_MAIN_LOGGER_ID,filter, log )
#define LOGW(filter, log ) LOG_WARN(SOUI::LOG4Z_MAIN_LOGGER_ID, filter,log )
#define LOGE(filter, log ) LOG_ERROR(SOUI::LOG4Z_MAIN_LOGGER_ID,filter, log )
#define LOGA(filter, log ) LOG_ALARM(SOUI::LOG4Z_MAIN_LOGGER_ID,filter, log )
#define LOGF(filter, log ) LOG_FATAL(SOUI::LOG4Z_MAIN_LOGGER_ID,filter, log )


//! format input log.
#ifdef LOG4Z_FORMAT_INPUT_ENABLE
#define LOG_FORMAT(id, level, filter, logformat, ...) \
	SOUI::SLogHelper::LogFormat(id,level,filter,__LINE__,__FUNCTION__,__FILE__,_ReturnAddress(),logformat, ##__VA_ARGS__)


//!format string
#define LOGFMT_TRACE(id_or_name, filter, fmt, ...)  LOG_FORMAT(id_or_name, SOUI::ILog4zManager::LOG_LEVEL_TRACE, filter, fmt, ##__VA_ARGS__)
#define LOGFMT_DEBUG(id_or_name, filter, fmt, ...)  LOG_FORMAT(id_or_name, SOUI::ILog4zManager::LOG_LEVEL_DEBUG, filter, fmt, ##__VA_ARGS__)
#define LOGFMT_INFO(id_or_name, filter, fmt, ...)  LOG_FORMAT(id_or_name, SOUI::ILog4zManager::LOG_LEVEL_INFO,  filter,fmt, ##__VA_ARGS__)
#define LOGFMT_WARN(id_or_name, filter, fmt, ...)  LOG_FORMAT(id_or_name, SOUI::ILog4zManager::LOG_LEVEL_WARN,  filter,fmt, ##__VA_ARGS__)
#define LOGFMT_ERROR(id_or_name, filter, fmt, ...)  LOG_FORMAT(id_or_name, SOUI::ILog4zManager::LOG_LEVEL_ERROR, filter, fmt, ##__VA_ARGS__)
#define LOGFMT_ALARM(id_or_name, filter, fmt, ...)  LOG_FORMAT(id_or_name, SOUI::ILog4zManager::LOG_LEVEL_ALARM, filter, fmt, ##__VA_ARGS__)
#define LOGFMT_FATAL(id_or_name, filter, fmt, ...)  LOG_FORMAT(id_or_name, SOUI::ILog4zManager::LOG_LEVEL_FATAL, filter, fmt, ##__VA_ARGS__)
#define LOGFMTT( filter, fmt, ...) LOGFMT_TRACE(SOUI::LOG4Z_MAIN_LOGGER_ID, filter, fmt,  ##__VA_ARGS__)
#define LOGFMTD( filter, fmt, ...) LOGFMT_DEBUG(SOUI::LOG4Z_MAIN_LOGGER_ID, filter, fmt,  ##__VA_ARGS__)
#define LOGFMTI( filter, fmt, ...) LOGFMT_INFO(SOUI::LOG4Z_MAIN_LOGGER_ID, filter, fmt,  ##__VA_ARGS__)
#define LOGFMTW( filter, fmt, ...) LOGFMT_WARN(SOUI::LOG4Z_MAIN_LOGGER_ID, filter, fmt,  ##__VA_ARGS__)
#define LOGFMTE( filter, fmt, ...) LOGFMT_ERROR(SOUI::LOG4Z_MAIN_LOGGER_ID, filter, fmt,  ##__VA_ARGS__)
#define LOGFMTA( filter, fmt, ...) LOGFMT_ALARM(SOUI::LOG4Z_MAIN_LOGGER_ID, filter, fmt,  ##__VA_ARGS__)
#define LOGFMTF( filter, fmt, ...) LOGFMT_FATAL(SOUI::LOG4Z_MAIN_LOGGER_ID, filter, fmt,  ##__VA_ARGS__)
#else
inline void empty_log_format_function1(LoggerId id, const char * tag, const char* fmt, ...){}
inline void empty_log_format_function1(const char * name, const char * tag, const char* fmt, ...){}
inline void empty_log_format_function1(LoggerId id, const char * tag, const wchar_t* fmt, ...){}
inline void empty_log_format_function1(const char * name, const char * tag, const wchar_t* fmt, ...){}
inline void empty_log_format_function2(const char * tag, const char* fmt, ...){}
inline void empty_log_format_function2(const char * tag, const wchar_t* fmt, ...){}
#define LOGFMT_TRACE empty_log_format_function1
#define LOGFMT_DEBUG LOGFMT_TRACE
#define LOGFMT_INFO LOGFMT_TRACE
#define LOGFMT_WARN LOGFMT_TRACE
#define LOGFMT_ERROR LOGFMT_TRACE
#define LOGFMT_ALARM LOGFMT_TRACE
#define LOGFMT_FATAL LOGFMT_TRACE
#define LOGFMTT empty_log_format_function2
#define LOGFMTD LOGFMTT
#define LOGFMTI LOGFMTT
#define LOGFMTW LOGFMTT
#define LOGFMTE LOGFMTT
#define LOGFMTA LOGFMTT
#define LOGFMTF LOGFMTT
#endif

namespace SOUI {
	//! optimze from std::stringstream to Log4zStream
#if defined (WIN32) || defined(_WIN64)
#pragma warning(push)
#pragma warning(disable:4996)
#endif

	class Log4zBinary
	{
	public:
		Log4zBinary(const char * buf, int len)
		{
			_buf = buf;
			_len = len;
		}
		const char * _buf;
		int  _len;
	};


	class SOUI_EXP Log4zStream
	{
	public:
		Log4zStream(char * buf, int len);
		int getCurrentLen() { return (int)(_cur - _begin); }
	private:
		Log4zStream & writeData(const char * ft, ...);
		Log4zStream & writeLongLong(long long t);
		Log4zStream & writeULongLong(unsigned long long t);
		Log4zStream & writePointer(const void * t);
		Log4zStream & writeString(const char* t);
		Log4zStream & writeWString(const wchar_t* t);
		Log4zStream & writeBinary(const Log4zBinary & t);
	public:
		const char * c_str() const;
		Log4zStream & operator <<(const void * t) { return  writePointer(t); }

		Log4zStream & operator <<(const char * t) { return writeString(t); }
#if defined (WIN32) || defined(_WIN64)
		Log4zStream & operator <<(const wchar_t * t) { return writeWString(t); }
#endif
		Log4zStream & operator <<(bool t) { return (t ? writeData("%s", "true") : writeData("%s", "false")); }

		Log4zStream & operator <<(char t) { return writeData("%c", t); }

		Log4zStream & operator <<(unsigned char t) { return writeData("%u", (unsigned int)t); }

		Log4zStream & operator <<(short t) { return writeData("%d", (int)t); }

		Log4zStream & operator <<(unsigned short t) { return writeData("%u", (unsigned int)t); }

		Log4zStream & operator <<(int t) { return writeData("%d", t); }

		Log4zStream & operator <<(unsigned int t) { return writeData("%u", t); }

		Log4zStream & operator <<(long t) { return writeLongLong(t); }

		Log4zStream & operator <<(unsigned long t) { return writeULongLong(t); }

		Log4zStream & operator <<(long long t) { return writeLongLong(t); }

		Log4zStream & operator <<(unsigned long long t) { return writeULongLong(t); }

		Log4zStream & operator <<(float t) { return writeData("%.4f", t); }

		Log4zStream & operator <<(double t) { return writeData("%.4lf", t); }

		Log4zStream & operator << (const Log4zBinary & binary) { return writeBinary(binary); }

	private:
		Log4zStream() {}
		Log4zStream(Log4zStream &) {}
		char *  _begin;
		char *  _end;
		char *  _cur;
	};

#if defined (WIN32) || defined(_WIN64)
#pragma warning(pop)
#endif

}//end of namespace SOUI
