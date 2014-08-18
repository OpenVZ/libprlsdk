/*
 * Logging.h: Unified logging library for all user-space components.
 *
 *
 * Copyright (C) 1999-2014 Parallels IP Holdings GmbH
 *
 * This file is part of Parallels SDK. Parallels SDK is free
 * software; you can redistribute it and/or modify it under the
 * terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License,
 * or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.  If not, see
 * <http://www.gnu.org/licenses/> or write to Free Software Foundation,
 * 51 Franklin Street, Fifth Floor Boston, MA 02110, USA.
 *
 * Our contact details: Parallels IP Holdings GmbH, Vordergasse 59, 8200
 * Schaffhausen, Switzerland; http://www.parallels.com/.
 */

/*
 * Logging is devided into two main parts - user & developer.
 * Logging interfaces are as the following:
 *
 *  *** LOGGING METHODS ***
 *
 * 0) LOG_MESSAGE      - VISIBLE ONLY FOR DEVELOPER
 * 1) WRITE_TRACE      - VISIBLE FOR THE END USER (!)
 *
 * Signature of the every logging function is the following:
 *     <function> ( level, "format string", ... )
 *
 *  *** LOGGING LEVELS ***
 *
 * Logging levels are:
 *
 *     (0) DBG_FATAL       - most important
 *     (1) DBG_WARNING     - almost most important
 *     (2) DBG_INFO        - almost least important
 *     (3) DBG_DEBUG       - least important
 *     (4) DBG_TRACE       - detailed information about function calls
 *
 * *** CUSTOMIZATION ***
 *
 * Users can force some parameters (there will be more in the future), namely:
 *
 * #define FORCE_LOGGING_ON
 * #define FORCE_LOGGING_LEVEL     DBG_DEBUG
 * #define FORCE_LOGGING_PREFIX    " >>>>>> MY >>>>> "
 *
 * And include library header after that
 *
 * #include "Libraries/Logging/Logging.h"
 *
 * Do not reinvent the wheel - use this module.
 * Fill free to extend it if you feel that you need this.
 */

#ifndef __LOGGING_H__
#define __LOGGING_H__

// Configuration variables are stored in the separated file
#include "Libraries/Logging/LoggingConfig.h"

// For va_arg parameters
#include <stdarg.h>

#ifndef PRINTABLE_TARGET
#define PRINTABLE_TARGET	_TARGET_
#endif

/**
 * Users of the module may request to force logging on in their module.
 * #define FORCE_LOGGING_ON
 * before the include of
 * #include "Libraries/Logging/Logging.h"
 */
#ifndef LOGGING_ON
	#ifdef FORCE_LOGGING_ON
		#define LOGGING_ON
	#endif
#endif

/**
 * Users of the module may request to force logging off in their module.
 * #define FORCE_LOGGING_OFF
 * before the include of
 * #include "Libraries/Logging/Logging.h"
 */
#ifdef LOGGING_ON
	#ifdef FORCE_LOGGING_OFF
		#undef LOGGING_ON
	#endif
#endif

/**
 * If Log4CXX is enabled, logging is always ON.
 */
#ifdef ENABLE_LOG4CXX
	#ifndef LOGGING_ON
		#define LOGGING_ON
	#endif
#endif

/**
 * Users of the module may request desired logging level with
 * #define FORCE_LOGGING_LEVEL 10
 * before the include of
 * #include "Libraries/Logging/Logging.h"
 */
#ifndef FORCE_LOGGING_LEVEL
	#define FORCE_LOGGING_LEVEL	0
#endif

/**
 * Users of the module may request desired log line prefix
 * #define FORCE_LOGGING_PREFIX  " >>>> MY LOG >>> "
 * before the include of
 * #include "Libraries/Logging/Logging.h"
 */
#ifndef FORCE_LOGGING_PREFIX
	#define FORCE_LOGGING_PREFIX ""
#endif

#ifdef __cplusplus
/**
 * BEWARE: For Qt users only.
 * Often used to forward all Qt messages to this logging system.
 * You should use this macros after all includes in cpp, but before
 * 'main' function, in which QApplication is declared.
 * @code
 * #include <QObject>
 * #include <some_header.h>
 *
 * // Forward all Qt msgs to logging system
 * INSTALL_LOGGING_QT_MSG_HANDLER(MyHandler)
 *
 * int main ( int argc, cahr** argv )
 * {
 * QApplication app(argc, argv);
 * return app.exec();
 * }
 *
 *
 * @endcode
 */
#ifdef QT_VERSION_CHECK

#if QT_VERSION >= QT_VERSION_CHECK( 5, 0, 0 )
#define Q_INSTALL_MESSAGE_HANDLER qInstallMessageHandler
#else
#define Q_INSTALL_MESSAGE_HANDLER qInstallMsgHandler
#endif // QT_VERSION >= QT_VERSION_CHECK( 5, 0, 0 )

#define COMMON_LOGGING_QT_MSG_HANDLER(TARGET)			 \
void TARGET ## MessageOutput ( QtMsgType type, const char* msg ) \
{																 \
	switch (type) {												 \
	case QtDebugMsg:											 \
		LOG_MESSAGE( DBG_QT_DEBUG, "%s", msg );					 \
		break;													 \
	case QtWarningMsg:											 \
		LOG_MESSAGE( DBG_QT_WARNING, "%s", msg );				 \
		break;													 \
	case QtCriticalMsg:											 \
		WRITE_TRACE( DBG_QT_FATAL, "%s", msg );					 \
		break;													 \
	case QtFatalMsg:											 \
		WRITE_TRACE( DBG_QT_FATAL, "%s", msg );					 \
		break;													 \
	}															 \
}

#define COMMON_INSTALL_LOGGING_QT_MSG_HANDLER(TARGET) \
class TARGET ## _QtMsgHandler  \
{ public: TARGET ## _QtMsgHandler () \
          { Q_INSTALL_MESSAGE_HANDLER(TARGET ## MessageOutput); } \
}; static TARGET ## _QtMsgHandler TARGET ## qtMsgHandler;

#if QT_VERSION >= QT_VERSION_CHECK( 5, 0, 0 )
// Definition for Qt5
#define INSTALL_LOGGING_QT_MSG_HANDLER(TARGET) \
COMMON_LOGGING_QT_MSG_HANDLER(TARGET) \
void TARGET ## Qt5MessageOutput ( QtMsgType type, const QMessageLogContext&, const QString& msg ) \
{ \
	TARGET ## MessageOutput( type, msg.toUtf8().constData() ); \
} \
COMMON_INSTALL_LOGGING_QT_MSG_HANDLER(TARGET ## Qt5)
#else
// Definition for Qt4
#define INSTALL_LOGGING_QT_MSG_HANDLER(TARGET) \
COMMON_LOGGING_QT_MSG_HANDLER(TARGET) \
COMMON_INSTALL_LOGGING_QT_MSG_HANDLER(TARGET)
#endif // QT_VERSION >= QT_VERSION_CHECK( 5, 0, 0 )

#endif // QT_VERSION_CHECK

#endif /* for Qt logging only, no need in C environment */


/**
 * Define special __PRL_FUNCTION__ for LOG4CXX, which contains full name:
 * 'type Class::method ( type )' or 'type function ( type )'
 */
#if defined(_MSC_VER)
#if _MSC_VER >= 1300
 #if defined(ENABLE_LOG4CXX)
      #define __PRL_FUNCTION__ __FUNCSIG__
 #else
      #define __PRL_FUNCTION__ __FUNCTION__
 #endif
#endif
#else
#if defined(__GNUC__)
 #if defined(ENABLE_LOG4CXX)
      #define __PRL_FUNCTION__ __PRETTY_FUNCTION__
 #else
      #define __PRL_FUNCTION__ __FUNCTION__
 #endif
#endif
#endif
#if !defined(__PRL_FUNCTION__)
#define __PRL_FUNCTION__ ""
#endif

#define TO_STR2(t) #t
#define TO_STR(t) TO_STR2(t)


/** Actual logging functions */

#ifdef __cplusplus
extern "C" {
#endif

extern int __log_level;		/* global log level of the app */

extern void log_release(const char *prefix, const char *target,
							int level, const char* fmt, ...)
#if defined(__GNUC__)
	__attribute__ ((format(printf, 4, 5)))
#endif
	;
extern  void log_debug(const char *file, int line, const char *function,
						  const char *prefix, const char *target,
						  int level, const char* fmt, ...)
#if defined(__GNUC__)
	__attribute__ ((format(printf, 7, 8)))
#endif
	;
typedef void (*LogMsgFunc_t)(const char *const msg, int length);

/**
 * Enable/disable logging to console. Provided for command line tools
 * return previous value
 */
extern int SetConsoleLogging(int enable);

/**
 * Set path and name of the log file.
 */
extern void SetLogFileName(const char* path, const char *name);

/**
 * Enforce current loglevel for the library
 */
extern void SetLogLevel( int level );

/**
 * Get current loglevel for the library
 */
extern int	GetLogLevel();

/**
 * Rate limiter structure - to be used
 * with Logger class and according macroses -
 *
 *	LOG_MESSAGE()
 *	WRITE_TRACE()
 *
 *	Example:
 *		// not faster then once in 3 seconds
 *		WRITE_TRACE_RL(3, DBG_FATAL, "...");
 */
struct LogRateLimit
{
	unsigned rate;	// limit of rate in seconds
	unsigned last;	// in seconds, last output timestamp
};

int LogCheckModifyRate(struct LogRateLimit *rl);

/**
 * Return current file path to which logs are written.
 */
const char *GetLogFileName();

/**
 * Return current dir to which logs are written.
 */
const char *GetLogFilePath();

/**
 * Return default dir to stor log files.
 */
const char *GetDefaultLogFilePath();

/**
 * Return user home directory to store user log file
 */
const char *GetUserHomeDir();

/**
 * Return product-specific default log file name.
 */
const char *GetProdDefaultLogFileName();

#ifdef __cplusplus
}

	/**
	 * We're using this strange (at the first sight) construction to convert
	 * #define PRINTABLE_TARGET name
	 * To the printable representation at compile-time
	 */
	struct Logger
	{
		const char*	m_File;
		int			m_Line;
		const char*	m_Func;

		const char*	m_ForcePrefix;
		const char*	m_Target;

		Logger(
			const char*	 File,
			int			 Line,
			const char*	 Func,
			const char*	 ForcePrefix,
			const char*	 Target
		)
			:	m_File(File),
				m_Line(Line),
				m_Func(Func),
				m_ForcePrefix(ForcePrefix),
				m_Target(Target)
		{
		}

		/**
		 * Reset file where logging is being performed.
		 * This is extremely useful to be done on the first system startup.
		 */
        enum LOG_RESET_REASON
        {
            LRR_RESTART,
            LRR_SIZE_LIMIT
        };
		static void ResetLogFile( Logger::LOG_RESET_REASON reason = LRR_RESTART );

		/**
		 * Puts message in log files, std output and debug output (if any)
		 */
		static void PutMessage(const char* const msg, int size);

		/**
		 * Install default logger callback. This callback will be called
		 * for each message to be processed
		 */
		static LogMsgFunc_t SetMessageCallback(LogMsgFunc_t logMsgFunc);

		int DefaultParseMessage(int level, const char* fmt, va_list args);
		int ParseMessage( int Level, const char* fmt, va_list args );

/** Actual logging functions */
friend void log_release(const char *prefix, const char *target,
						int level, const char* fmt, ...);
friend void log_debug(const char *file, int line, const char *function,
					  const char *prefix, const char *target,
					  int level, const char* fmt, ...);
	};
#endif /* __cplusplus */


/**
 * LOG_MESSAGE interface definition
 */
#ifdef LOGGING_ON

/**
 * Developer build logging.
 * These logging enabled only in the development build and does not go to
 * the customer. Default logging level is defined with FORCE_LOGGING_LEVEL
 * macro in the LoggingConfig.h file.
 */
#define LOG_MESSAGE(level, ...)	WRITE_TRACE(level, __VA_ARGS__)
#define __log_helper
#else
/* Used to check arguments validity in LOG_MESSAGE/LOG_TIMED_MESSAGE
 * xxx is required to make gcc happy */
static __inline int __dummy_log_message__(int xxx, ...)
{
	(void)xxx;
	return 0;
}

#define LOG_MESSAGE(level, ...)			do {				\
	(void)sizeof(__dummy_log_message__(0, __VA_ARGS__));	\
} while (0)
#define __log_helper	__inline __used
#endif


/** Write trace with rate limitator */
#define WRITE_TRACE_RL(rate, level, ...) do {						\
	static struct LogRateLimit lrl = {rate, (unsigned)-1};			\
	if (!LogCheckModifyRate(&lrl))									\
		break;														\
	WRITE_TRACE(level, __VA_ARGS__);								\
} while (0)


/**
 * WRITE_TRACE interface definition
 *
 * User build logging.
 * This logging is always enabled and visible to the user.
 * Default logging level is defined with FORCE_LOGGING_LEVEL macro in
 * the LoggingConfig.h file.
 */

#if defined(LOGGING_ON) || defined(_DEBUG)

#define WRITE_TRACE(level, ...)	do {										\
	if ((int)((level) % DBG_GROUP_SIZE) > FORCE_LOGGING_LEVEL &&			\
			(int)((level) % DBG_GROUP_SIZE) > __log_level)					\
		break;																\
	log_debug(__FILE__, __LINE__, __PRL_FUNCTION__, FORCE_LOGGING_PREFIX,	\
			  TO_STR(PRINTABLE_TARGET), (level), __VA_ARGS__);				\
} while (0)

#else

#define WRITE_TRACE(level, ...)		do {									\
	if ((int)((level) % DBG_GROUP_SIZE) > FORCE_LOGGING_LEVEL &&			\
			(int)((level) % DBG_GROUP_SIZE) > __log_level)					\
		break;																\
	log_release(FORCE_LOGGING_PREFIX, TO_STR(PRINTABLE_TARGET),				\
				(level), __VA_ARGS__);										\
} while (0)

#endif

/**
 * PUT_RAW_MESSAGE
 */
#define PUT_RAW_MESSAGE(msg)	Logger::PutMessage(msg, strlen(msg))

#ifdef _WIN_
extern void (*etrace_log_func)(unsigned int src, unsigned int type, unsigned __int64 data);
#else
extern void (*etrace_log_func)(unsigned int src, unsigned int type, unsigned long long data);
#endif

#endif // __LOGGING_H__
