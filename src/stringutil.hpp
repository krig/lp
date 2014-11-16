#pragma once

namespace util {
	using std::string;
	using std::stringstream;
	typedef std::vector<string> stringvec;

	inline
	stringvec split(const string& s, const string& sep = " ") {
		stringvec ret;
		typedef char_separator<char> sep_t;
		typedef tokenizer<sep_t> tok_t;
		sep_t sp(sep.c_str());
		tok_t tok(s, sp);
		for (const auto& token : tok)
			ret.push_back(token);
		return ret;
	}

	inline
	bool endswith(const string& s, const char* end) {
		const size_t elen = strlen(end);
		if (s.length() <= elen)
			return false;
		return s.compare(s.length()-elen, elen, end) == 0;
	}

	// Replacements for str(n)cpy, str(n)cat
	// Rationale: http://byuu.org/articles/programming/strcpycat
	// length argument includes null-terminator
	// returns: strlen(target)
	inline
	unsigned strmcpy(char *target, const char *source, unsigned length) {
		const char *origin = target;
		if(length) { while(*source && --length) *target++ = *source++; *target = 0; }
		return target - origin;
	}

	// Replacements for str(n)cpy, str(n)cat
	// Rationale: http://byuu.org/articles/programming/strcpycat
	// length argument includes null-terminator
	// returns: strlen(target)
	inline
	unsigned strmcat(char *target, const char *source, unsigned length) {
		const char *origin = target;
		while(*target && length) target++, length--;
		return (target - origin) + strmcpy(target, source, length);
	}
}
