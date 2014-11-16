#include "common.hpp"
#include "file.hpp"

namespace util {
	size_t file::size() const {
		if (_size == 0) {
			long now = tell();
			if (now == -1)
				throw std::runtime_error("ftell() returned -1");
			fseek(_file, 0L, SEEK_END);
			long end = tell();
			if (end == -1)
				throw std::runtime_error("ftell() returned -1");
			fseek(_file, now, SEEK_SET);
			_size = (size_t)end;
		}
		return _size;
	}

	// Read entire file into memory
	// This won't open files that are too big.
	// Too big meaning > 1GB.
	static const ssize_t MAX_FILE_SIZE = 1024*1024*1024;

	string read_file(const char* fname) {
		file f(fname, "r");
		if (f.open()) {
			ssize_t sz = f.size();
			if (sz > 0 && sz <= MAX_FILE_SIZE) {
				std::vector<char> contents;
				contents.reserve(sz+1);
				size_t amt = f.read(contents, sz);
				contents.resize(sz+1);
				contents[amt] = 0;
				return string(&contents.front());
			}
		}
		return "";
	}
}
