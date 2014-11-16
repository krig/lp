#include <stdint.h>
#include <time.h>
#include <sys/time.h>
#include <algorithm>

extern "C" int crypto_auth( unsigned char *out, const unsigned char *in, unsigned long long inlen, const unsigned char *k );

namespace {
	uint8_t s_hashseed[16];
}

namespace util {
	void hash_seed() {
		timeval tv;
		gettimeofday(&tv, nullptr);
		uint32_t* rw = (uint32_t*)s_hashseed;
		rw[0] = tv.tv_usec;
		rw[1] = tv.tv_sec;
		rw[2] = tv.tv_usec;
		rw[3] = tv.tv_sec;
	}

	uint64_t hash_string(const char* s, size_t len) {
		uint64_t out;
		crypto_auth((uint8_t*)&out, (const uint8_t*)s, len, s_hashseed);
		return std::move(out);
	}
}
