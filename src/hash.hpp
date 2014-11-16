#pragma once

namespace util {
	void hash_seed();
	uint64_t hash_string(const char* s, size_t len);
}

extern "C" int crypto_auth( unsigned char *out, const unsigned char *in, unsigned long long inlen, const unsigned char *k );

