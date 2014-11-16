#pragma once

#include <sstream>
#include <string>

namespace util {
	inline int from_hex( char c ) {
		if ( '0' <= c && c <= '9' )
			return c - '0';
		if ( 'a' <= c && c <= 'f' )
			return c - 'a' + 10;
		if ( 'A' <= c && c <= 'F' )
			return c - 'A' + 10;
		return 0xff;
	}
	inline char from_hex( const char *c ) {
		return (char)(( from_hex( c[ 0 ] ) << 4 ) | from_hex( c[ 1 ] ));
	}

	inline string hexdump(const void* inRaw, int len) {
		static const char hexchars[] = "0123456789abcdef";

		std::stringstream out;
		auto in = reinterpret_cast<const char*>(inRaw);
		for (int i=0; i<len; ++i) {
			char c = in[i];
			char hi = hexchars[(c & 0xF0) >> 4];
			char lo = hexchars[(c & 0x0F)];

			out << hi << lo;
		}

		return out.str();
	}
}
