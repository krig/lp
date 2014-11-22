#pragma once

#include <zlib.h>

struct gzfile {
	static const int BufferSize = 256*1024;

	gzfile(const char* filename, const char* mode = "r") : _fil(0) {
		_fil = gzopen(filename, mode);
		_buf.resize(BufferSize);
		_head = _buf.begin();
		_end = _buf.begin();
	}

	~gzfile() {
		gzclose(_fil);
	}

	const char* gets() {
		if (_head == _end)
			refill(_head);

		const char* start = _head;
		int scans = 0;

	rescan:
		if (scans > 1) {
			_head = _end;
			return start;
		}
		while (*_head != '\n' && (_head < _end))
			++_head;

		if (_head == _end) {
			scans++;
			refill(start);
			start = _buf.buffer();
			goto rescan;
		}

		*_head++ = 0;
		return start;
	}

	bool eof() const {
		return (_head == _end) && gzeof(_fil);
	}

private:
	void refill(const char* start) {
		size_t headoffs = _head - start;
		size_t endoffs = _end - start;
		if (start < _end) {
			if (_buf.buffer() + endoffs < start) {
				memcpy(_buf.buffer(), start, endoffs);
			}
			else {
				memmove(_buf.buffer(), start, endoffs);
			}
		}
		int amount = gzread(_fil, _buf.buffer() + endoffs, _buf.capacity() - endoffs);
		_head = _buf.buffer() + headoffs;
		_end = _buf.buffer() + endoffs + amount;
	}

	gzFile _fil;
	string _buf;
	char* _head;
	char* _end;
};
