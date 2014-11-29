#include "common.hpp"
#include "intern.hpp"

namespace {

	struct hash_str {
		size_t operator()(const char* str) const {
			size_t h = 0;
			for (size_t i = strlen(str); i != 0; --i)
				h = 31 * h + str[i];
			return h;
		}
	};

	struct str_equal {
		bool operator()(const char* a, const char* b) const {
			return strcmp(a, b) == 0;
		}
	};

	std::unordered_set<const char*, hash_str, str_equal> _interned;

	const char* copy_string(const char* str)
	{
		// TODO: block allocate from arena allocator?
		// as long as I never free these strings, it
		// doesn't really matter much.
		return strdup(str);
	}
}

const char* intern_string(const char* str, bool copy)
{
	auto i = _interned.find(str);
	if (i == _interned.end()) {
		if (copy)
			str = copy_string(str);
		_interned.insert(str);
		return str;
	}
	return *i;
}
