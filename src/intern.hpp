#pragma once

// string interning

struct symbol {
	const char* _text;

	symbol() {}
	symbol(const char* s) : _text(s) {}
	symbol(const symbol& s) : _text(s._text) {}

	symbol& operator=(const char* s) {
		_text = s;
		return *this;
	}

	symbol& operator=(const symbol& s) {
		_text = s._text;
		return *this;
	}

	operator const char* () const {
		return _text;
	}

	bool operator==(const char* str) const {
		return strcmp(_text, str) == 0;
	}

	bool operator!=(const char* str) const {
		return strcmp(_text, str) != 0;
	}

	size_t length() const {
		return strlen(_text);
	}

	const char& operator[](int idx) const {
		return _text[idx];
	}

	const char* operator()() const {
		return _text;
	}

	const char* str() const {
		return _text;
	}
};

symbol intern_string(const char* str, bool copy);
