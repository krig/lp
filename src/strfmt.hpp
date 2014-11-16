#pragma once

#include <stdarg.h>

namespace util {
	template <int SIZE = 256>
	struct strfmt
	{
		char tmp[SIZE];
		strfmt(const char* fmt, ...) {
			va_list va_args;
			va_start(va_args, fmt);
			vsnprintf(tmp, SIZE, fmt, va_args);
			va_end(va_args);
		}
		operator const char* () const { return tmp; }

		const char* c_str() const { return tmp; }
		string str() const { return string(tmp); }
	};
}
