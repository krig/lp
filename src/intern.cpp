#include "common.hpp"
#include "intern.hpp"

namespace {
	struct hash_str {
		size_t operator()(const symbol& str) const {
			size_t h = 0;
			for (size_t i = str.length(); i != 0; --i)
				h = 31 * h + str[i-1];
			return h;
		}
	};

	struct str_equal {
		bool operator()(const symbol& a, const symbol& b) const {
			return a == b;
		}
	};

	std::unordered_set<symbol, hash_str, str_equal> _interned;
}

symbol intern_string(const char* str, bool copy)
{
	auto i = _interned.find(symbol(str));
	if (i == _interned.end())
		i = _interned.emplace(copy ? strdup(str) : str).first;
	return *i;
}
