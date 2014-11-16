#pragma once

#define UTIL_CHECK_PRINTF(fmt, idx) __attribute__ ((format (printf, fmt, idx)))

namespace util {
	namespace logging {
		enum Colors { RED = 91, GREEN, YELLOW, BLUE, MAGENTA, CYAN, WHITE };

		extern bool colors;

		enum ModeFlags {
			LogToConsole = 0x0001,
			LogToSyslog = 0x0002,
			LogToFile = 0x0004
		};

		enum LogLevels {
			Trace = 0,
			Info = 1,
			Warn = 2,
			Error = 3
		};

		void set_log_mode(int flags = LogToConsole);
		void set_log_level(LogLevels level);

		void cprintf(int color, const char* fmt, ...) UTIL_CHECK_PRINTF(2, 3);

		struct log_context {
			log_context(const char* fil, int lin, const char* fun);
			void trace(const char* fmt, ...) UTIL_CHECK_PRINTF(2, 3);
			void info(const char* fmt, ...) UTIL_CHECK_PRINTF(2, 3);
			void warn(const char* fmt, ...) UTIL_CHECK_PRINTF(2, 3);
			void error(const char* fmt, ...) UTIL_CHECK_PRINTF(2, 3);

			const char* _file;
			int _line;
			const char* _fun;
		};


		struct logproxy {
			static const int PROXY_BUFSIZE = 4096;

			struct threadlocal {
				logproxy* proxy;
				int outpipe;
				char buf1[PROXY_BUFSIZE]; // space for two lines
				char buf2[PROXY_BUFSIZE];
			};

			logproxy();
			~logproxy();
			bool create();
			void close();
			void run(threadlocal* data);

			int inpipe;

			char* _consume(char* wptr, ssize_t amt, char*const buf1, char*const buf2);
			void _eject(const char* str);
		};

	}
}

#define LOG_TRACE util::logging::log_context(__FILE__, __LINE__, __FUNCTION__).trace
#define LOG_INFO util::logging::log_context(__FILE__, __LINE__, __FUNCTION__).info
#define LOG_WARN util::logging::log_context(__FILE__, __LINE__, __FUNCTION__).warn
#define LOG_ERROR util::logging::log_context(__FILE__, __LINE__, __FUNCTION__).error
