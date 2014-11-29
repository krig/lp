#include "common.hpp"
#include "intern.hpp"

namespace {
	set<const char*> _interned;

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
