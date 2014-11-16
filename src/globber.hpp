#pragma once

#include <glob.h>

namespace util {
	struct globber {
		globber() {
			memset(&_glob, 0, sizeof(glob_t));
		}

		globber(const char* pattern) {
			memset(&_glob, 0, sizeof(glob_t));
			start(pattern);
		}

		~globber() {
			clear();
		}

		void start(const char* pattern) {
			glob(pattern, GLOB_TILDE|GLOB_NOSORT, 0, &_glob);
		}

		void append(const char* pattern) {
			glob(pattern, GLOB_TILDE|GLOB_NOSORT|GLOB_APPEND, 0, &_glob);
		}

		void clear() {
			if (_glob.gl_pathc) {
				globfree(&_glob);
				memset(&_glob, 0, sizeof(glob_t));
			}
		}

		size_t size() const {
			return _glob.gl_pathc;
		}

		const char* operator[](size_t index) const {
			return (index < _glob.gl_pathc)? _glob.gl_pathv[index] : "";
		}

		glob_t _glob;
	};

}
