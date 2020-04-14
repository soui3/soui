#include <souistd.h>
#include "helper/slog.h"

namespace SOUI
{
	ILog4zManager * SLogHelper::s_LogMgr = NULL;

	void SLogHelper::SetLogMgr(ILog4zManager *pLogMgr)
	{
		s_LogMgr = pLogMgr;
	}

	ILog4zManager * SLogHelper::GetLogMgr()
	{
		if(s_LogMgr)
			return s_LogMgr;
		return SOUI::SApplication::getSingletonPtr()?SOUI::SApplication::getSingleton().GetLogManager():NULL;
	}

	void SLogHelper::LogStream(int id,int level,const char* filter, int line , const char * func , const char * file,const void *pRetAddr,  Log4zStream & stream)
	{
		ILog4zManager * pLogMgr = GetLogMgr(); 
		if (pLogMgr && pLogMgr->prePushLog(id,level)) 
		{
			pLogMgr->pushLog(id, level, filter, stream.c_str(), file, line, func, pRetAddr);
		}else if(level>=OUTLOG_LEVEL)
		{
			stream<<" "<<func<<" "<<file<<":"<<line<<"n";
			OutputDebugStringA(stream.c_str());
		}
	}

	void SLogHelper::LogFormat(int id,int level,const char* filter, int line , const char * func , const char * file,const void *pRetAddr, const char *logformat, ...)
	{
		va_list args;
		va_start(args, logformat);
		_LogFormat(id,level,filter,line,func,file,pRetAddr,logformat,args);
		va_end(args);
	}


	void SLogHelper::LogFormat(int id,int level,const char* filter, int line , const char * func , const char * file,const void *pRetAddr, const wchar_t *logformat, ...)
	{
		va_list args;
		va_start(args, logformat);
		_LogFormat(id,level,filter,line,func,file,pRetAddr,logformat,args);
		va_end(args);
	}


	void SLogHelper::LogStream(const char* name,int level,const char* filter, int line , const char * func , const char * file,const void *pRetAddr,  Log4zStream & stream)
	{
		ILog4zManager * pLogMgr = GetLogMgr(); 
		int nId=-1;
		if(pLogMgr)
			nId = pLogMgr->findLogger(name);
		LogStream(nId,level,filter,line,func,file,pRetAddr,stream);
	}

	void SLogHelper::LogFormat(const char* name,int level,const char* filter, int line , const char * func , const char * file,const void *pRetAddr, const char *logformat, ...)
	{
		ILog4zManager * pLogMgr = GetLogMgr(); 
		int nId=-1;
		if(pLogMgr)
			nId = pLogMgr->findLogger(name);
		va_list args;
		va_start(args, logformat);
		_LogFormat(nId,level,filter,line,func,file,pRetAddr,logformat,args);
		va_end(args);
	}


	void SLogHelper::LogFormat(const char* name,int level,const char* filter, int line , const char * func , const char * file,const void *pRetAddr, const wchar_t *logformat, ...)
	{
		va_list args;
		va_start(args, logformat);
		int nId=-1;
		ILog4zManager * pLogMgr = GetLogMgr(); 
		if(pLogMgr)
			nId = pLogMgr->findLogger(name);
		_LogFormat(nId,level,filter,line,func,file,pRetAddr,logformat,args);
		va_end(args);
	}

	void SLogHelper::_LogFormat(int id,int level,const char* filter, int line , const char * func , const char * file,const void *pRetAddr, const char *logformat, va_list & args)
	{
		ILog4zManager * pLogMgr = GetLogMgr();
		char *logbuf=(char*)malloc(LOG4Z_LOG_BUF_SIZE+1);
		vsprintf_s(logbuf, LOG4Z_LOG_BUF_SIZE,logformat, args); 
		logbuf[LOG4Z_LOG_BUF_SIZE]=0;
		_PushLog(pLogMgr,logbuf,id,level,filter,line,func,file,pRetAddr);
		free(logbuf);
	}

	void SLogHelper::_LogFormat(int id,int level,const char* filter, int line , const char * func , const char * file,const void *pRetAddr, const wchar_t *logformat, va_list & args)
	{
		ILog4zManager * pLogMgr = GetLogMgr();
		char *logbuf=(char*)malloc(LOG4Z_LOG_BUF_SIZE+1);
		{
			wchar_t *logbufw = (wchar_t*)malloc(LOG4Z_LOG_BUF_SIZE*sizeof(wchar_t)); 
			int nLen=vswprintf(logbufw, LOG4Z_LOG_BUF_SIZE,logformat, args); 
			DWORD dwLen = WideCharToMultiByte(CP_ACP, 0, logbufw, nLen, NULL, 0, NULL, NULL);
			if (dwLen < LOG4Z_LOG_BUF_SIZE)
			{
				WideCharToMultiByte(CP_ACP, 0, logbufw, nLen, logbuf, dwLen, NULL, NULL);
				logbuf[dwLen]=0;
			}
			free(logbufw);
		}
		_PushLog(pLogMgr,logbuf,id,level,filter,line,func,file,pRetAddr);
		free(logbuf);
	}

	void SLogHelper::_PushLog(ILog4zManager * pLogMgr,const char *logbuf,int id,int level,const char* filter, int line , const char * func , const char * file,const void *pRetAddr)
	{
		if (pLogMgr && pLogMgr->prePushLog(id,level)) 
		{
			pLogMgr->pushLog(id, level,filter, logbuf,file , line, func,pRetAddr); 
		}else if(level>=OUTLOG_LEVEL)
		{
			char *logbuf2 = (char*)malloc(LOG4Z_LOG_BUF_SIZE+1);
			_snprintf_s(logbuf2, LOG4Z_LOG_BUF_SIZE, _TRUNCATE, "%s %s %s:%d\n",logbuf, func, file, line ); 
			OutputDebugStringA(logbuf2);
			free(logbuf2);
		}
	}

	//////////////////////////////////////////////////////////////////////////

	Log4zStream::Log4zStream(char * buf, int len)
	{
		_begin = buf;
		_end = buf + len;
		_cur = _begin;
	}

	Log4zStream & Log4zStream::writeData(const char * fmt,...)
	{
		va_list args;
		va_start(args,fmt);
		if (_cur < _end)
		{
			int len = 0;
			int count = (int)(_end - _cur)-1;
#if defined (WIN32) || defined(_WIN64)
			len = _vsnprintf(_cur, count, fmt, args);
			if (len == count || (len == -1 && errno == E_RANGE))
			{
				len = count;
				*(_end - 1) = '\0';
			}
			else if (len < 0)
			{
				*_cur = '\0';
				len = 0;
			}
#else
			len = vsnprintf(_cur, count, fmt, args);
			if (len < 0)
			{
				*_cur = '\0';
				len = 0;
			}
			else if (len >= count)
			{
				len = count;
				*(_end - 1) = '\0';
			}
#endif
			_cur += len;
		}
		va_end(args);

		return *this;
	}

	Log4zStream & Log4zStream::writeLongLong(long long t)
	{
#if defined (WIN32) || defined(_WIN64) 
		writeData("%I64d", t);
#else
		writeData("%lld", t);
#endif
		return *this;
	}

	Log4zStream & Log4zStream::writeULongLong(unsigned long long t)
	{
#if defined (WIN32) || defined(_WIN64) 
		writeData("%I64u", t);
#else
		writeData("%llu", t);
#endif
		return *this;
	}

	Log4zStream & Log4zStream::writePointer(const void * t)
	{
#if defined (WIN32) || defined(_WIN64)
		sizeof(t) == 8 ? writeData("%016I64x", (unsigned long long)t) : writeData("%08I64x", (unsigned long long)t);
#else
		sizeof(t) == 8 ? writeData("%016llx", (unsigned long long)t) : writeData("%08llx", (unsigned long long)t);
#endif
		return *this;
	}

	Log4zStream & Log4zStream::writeBinary(const Log4zBinary & t)
	{
		writeData("%s", "\r\n\t[");
		for (int i = 0; i < t._len; i++)
		{
			if (i % 16 == 0)
			{
				writeData("%s", "\r\n\t");
				*this << (void*)(t._buf + i);
				writeData("%s", ": ");
			}
			writeData("%02x ", (unsigned char)t._buf[i]);
		}
		writeData("%s", "\r\n\t]\r\n\t");
		return *this;
	}

	Log4zStream & Log4zStream::writeWString(const wchar_t* t)
	{
#if defined (WIN32) || defined(_WIN64)
		DWORD dwLen = WideCharToMultiByte(CP_ACP, 0, t, -1, NULL, 0, NULL, NULL);
		if (dwLen < SOUI::LOG4Z_LOG_BUF_SIZE)
		{
			char buf[SOUI::LOG4Z_LOG_BUF_SIZE];
			dwLen = WideCharToMultiByte(CP_ACP, 0, t, -1, buf, dwLen, NULL, NULL);
			if (dwLen > 0)
			{
				buf[dwLen] = 0;
				writeData("%s", buf);
			}
		}
#else
		//not support
#endif
		return *this;
	}

	Log4zStream & Log4zStream::writeString(const char* t)
	{
		writeData("%s", t);
		return *this;
	}

	const char * Log4zStream::c_str() const
	{
		return _begin;
	}

}
