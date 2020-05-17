#pragma once

#include <interface/obj-ref-i.h>
#include <time.h>

SNSBEGIN

typedef int LoggerId;

//! the max log content length.
enum{
	LOG4Z_LOG_BUF_SIZE = 10240,
	LOG4Z_MAIN_LOGGER_ID = 0,
};

//! LOG Level
enum ENUM_LOG_LEVEL
{
	LOG_LEVEL_TRACE = 0,
	LOG_LEVEL_DEBUG,
	LOG_LEVEL_INFO,
	LOG_LEVEL_WARN,
	LOG_LEVEL_ERROR,
	LOG_LEVEL_ALARM,
	LOG_LEVEL_FATAL,
};

#undef INTERFACE
#define INTERFACE IOutputFileBuilder
DECLARE_INTERFACE(IOutputFileBuilder)
{
	//每个月创建log文件夹
	STDMETHOD_(BOOL,monthDir)(THIS) SCONST PURE;

	//每天创建log文件
	STDMETHOD_(BOOL,dayLog)(THIS) SCONST PURE;

	//生成LOG文件名
	//至少应该包含pszLogName，及curFileIndex这两个参数
	STDMETHOD_(BOOL,buildOutputFile)(THIS_ char *pszFileName,int nLen,tm time,const char * pszLogName,unsigned long pid,int curFileIndex) SCONST PURE;
};

#undef INTERFACE
#define INTERFACE IOutputListener
DECLARE_INTERFACE(IOutputListener)
{
	STDMETHOD_(void,onOutputLog)(THIS_ int level, const char * filter, const char * log, int nLogLen,unsigned __int64 time_) PURE;
};

//! log4z class
#undef INTERFACE
#define INTERFACE ILog4zManager
DECLARE_INTERFACE_(ILog4zManager,IObjRef)
{
	//!添加引用
	/*!
	*/
	STDMETHOD_(long,AddRef) (THIS) PURE;

	//!释放引用
	/*!
	*/
	STDMETHOD_(long,Release) (THIS) PURE;

	//!释放对象
	/*!
	*/
	STDMETHOD_(void,OnFinalRelease) (THIS) PURE;

    //! Config or overwrite configure
    //! Needs to be called before ILog4zManager::Start,, OR Do not call.
    STDMETHOD_(BOOL,config)(THIS_ const char * configPath) PURE;
    STDMETHOD_(BOOL,configFromString)(THIS_ const char * configContent) PURE;

    //! Create or overwrite logger.
    //! Needs to be called before ILog4zManager::Start, OR Do not call.
    STDMETHOD_(LoggerId,createLogger)(THIS_ const char* key) PURE;

    //! Start Log Thread. This method can only be called once by one process.
    STDMETHOD_(BOOL,start)(THIS) PURE;

    //! Default the method will be calling at process exit auto.
    //! Default no need to call and no recommended.
    STDMETHOD_(BOOL,stop)(THIS) PURE;

    //! Find logger. thread safe.
    STDMETHOD_(LoggerId,findLogger)(THIS_ const char* key) PURE;

    //pre-check the log filter. if filter out return false. 
    STDMETHOD_(BOOL,prePushLog)(THIS_ LoggerId id, int level) PURE;
    STDMETHOD_(BOOL,prePushLog)(THIS_ const char * name, int level) PURE;
    
    //! Push log, thread safe.
    STDMETHOD_(BOOL,pushLog)(THIS_ LoggerId id, int level, const char * filter, const char * log, const char * file , int line , const char * func , const void *pRetAddr) PURE;

    STDMETHOD_(BOOL,pushLog)(THIS_ const char * name, int level, const char * filter, const char * log, const char * file , int line , const char * func , const void *pRetAddr) PURE;

    //! set logger's attribute, thread safe.
    STDMETHOD_(BOOL,enableLogger)(THIS_ LoggerId id, bool enable) PURE;
    STDMETHOD_(BOOL,setLoggerName)(THIS_ LoggerId id, const char * name) PURE;
    STDMETHOD_(BOOL,setLoggerPath)(THIS_ LoggerId id, const char * path) PURE;
    STDMETHOD_(BOOL,setLoggerLevel)(THIS_ LoggerId id, int nLevel) PURE;
    STDMETHOD_(BOOL,setLoggerFileLine)(THIS_ LoggerId id, bool enable) PURE;
    STDMETHOD_(BOOL,setLoggerDisplay)(THIS_ LoggerId id, bool enable) PURE;
    STDMETHOD_(BOOL,setLoggerOutFile)(THIS_ LoggerId id, bool enable) PURE;
    STDMETHOD_(BOOL,setLoggerLimitsize)(THIS_ LoggerId id, unsigned int limitsize) PURE;
    
    //设置LOG输出到文件的规则
    STDMETHOD_(void,setOutputFileBuilder)(THIS_ IOutputFileBuilder *pOutputFileBuilder) PURE;

    //! Update logger's attribute from config file, thread safe.
    STDMETHOD_(BOOL,setAutoUpdate)(THIS_ int interval/*per second, 0 is disable auto update*/) PURE;
    STDMETHOD_(BOOL,updateConfig)(THIS) PURE;

    //! Log4z status statistics, thread safe.
    STDMETHOD_(BOOL,isLoggerEnable)(THIS_ LoggerId id) PURE;
    STDMETHOD_(unsigned int,getStatusActiveLoggers)(THIS) PURE;
	STDMETHOD_(void,setOutputListener)(THIS_ IOutputListener *pListener) PURE;
};

SNSEND