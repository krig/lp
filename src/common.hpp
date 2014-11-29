#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <vector>
#include <array>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <utility>

// pull some commonly used stuff into our namespace
using std::string;
using std::move;
using std::swap;
using std::make_pair;
template <typename T> using vector = std::vector<T>;
template <typename T, size_t N> using array = std::array<T, N>;
template <typename P> using unique_ptr = std::unique_ptr<P>;
template <typename F, typename S> using pair = std::pair<F, S>;
template <typename K, typename V> using dict = std::unordered_map<K, V>;
template <typename V> using set = std::unordered_set<V>;

#define ASIZE(a) (sizeof(a)/sizeof((a)[0]))

struct error : public std::exception {
	error(const char* fmt, ...) {
		va_list va_args;
		va_start(va_args, fmt);
		vsnprintf(_buf, 2048, fmt, va_args);
		va_end(va_args);
	}

	virtual const char* what() const noexcept {
		return _buf;
	}

private:
	char _buf[2048];
};

namespace std {
	inline vector<string>& operator<<(vector<string>& v, const char* t) {
		v.push_back(t);
		return v;
	}
}

template <size_t Pad = 4>
inline void bytevector_pad(vector<uint8_t>& vec) {
	const size_t size = vec.size();
	if (size & (Pad - 1))
		vec.resize((size & ~(Pad - 1)) + Pad, 0);
}

template <typename I>
inline void delete_all(I begin, I end) {
	for (I i = begin; i != end; ++i)
		delete *i;
}

#include "strfmt.hpp"
#include "logging.hpp"


#define ASSERT3(x, msg, file, line) \
        throw error("%s(%d) Assert [" #x "] %s", file, line, msg)

#define ASSERT(x) do { if (!(x)) { ASSERT3(x, "", __FILE__, __LINE__); } } while (0)
#define ASSERT2(x, msg) do { if (!(x)) { ASSERT3(x, msg, __FILE__, __LINE__); } } while (0)

extern "C" int crypto_auth( unsigned char *out, const unsigned char *in, unsigned long long inlen, const unsigned char *k );
