#include "common.hpp"
#include "intern.hpp"

namespace {
	struct interned_string {
		interned_string(const char* str, bool copy) : str(str), copy(copy) {
			if (copy)
				this->str = strdup(str);
		}
		// TODO: uncomment this if I ever decide to enable uninterning strings
		//~interned_string() {
			//if (copy)
			//	free((void*)str);
		//}
		const char* str;
		bool copy;
	};

	struct hash_str {
		size_t operator()(const interned_string& str) const {
			size_t h = 0;
			for (size_t i = strlen(str.str); i != 0; --i)
				h = 31 * h + str.str[i];
			return h;
		}
	};

	struct str_equal {
		bool operator()(const interned_string& a, const interned_string& b) const {
			return strcmp(a.str, b.str) == 0;
		}
	};

	std::unordered_set<interned_string, hash_str, str_equal> _interned;
}

const char* intern_string(const char* str, bool copy)
{
	auto i = _interned.find(interned_string(str, false));
	if (i == _interned.end())
		i = _interned.emplace(str, copy).first;
	return i->str;
}
