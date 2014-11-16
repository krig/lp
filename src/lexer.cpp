#include "common.hpp"
#include "file.hpp"
#include "lexer.hpp"

const char* token_name[] = {
#define TOKEN(t) #t
	TOKENS
#undef TOKEN
};


void lexer_state::init(const char* filename)
{
	_file = filename;
	_offset = 0;
	_line = 1;
	_column = 0;
	_text = read_file(filename);
}

#define NEXT { ++_offset; ++_column; ch = _text[_offset]; }
#define PEEK _text[_offset + 1]
#define NEXTLINE { _column = 0; ++_line; ++_offset; ch = _text[_offset]; }

namespace {
	set<string> keywords {
		"return",
		"if",
		"then",
		"else",
		"for",
		"match",
		"use",
		"struct",
		"enum",
		"union",
		"sizeof",
	};

	inline bool isident0(int ch) {
		return isalpha(ch) || ch == '_';
	}

	inline bool isidentn(int ch) {
		return isalnum(ch) || ch == '_';
	}

	inline bool isnumeric(int ch) {
		return isdigit(ch) || ch == '_' || ch == '.' || ch == 'u' || ch == 'f' || ch == 'l';
	}
}

token* lexer_state::next_token()
{
	int ch;
	token* ret;

	while (_offset < _text.size()) {
		ch = _text[_offset];
		if (ch == '\n') {
			NEXTLINE;
			continue;
		}
		if (isspace(ch)) {
			NEXT;
			continue;
		}
		if (ch == '#') {
			while (ch != '\n' && _offset < _text.size()) {
				NEXT;
			}
			if (ch == '\n') {
				NEXTLINE;
				continue;
			}
		}
		if (ch == ':') {
			NEXT;
			if (ch == ':') {
				ret = new token(T_DOUBLE_COLON, "::", _file, _line, _column-1);
				NEXT;
				return ret;
			} else if (ch == '=') {
				ret = new token(T_COLON_ASSIGN, ":=", _file, _line, _column-1);
				NEXT;
				return ret;
			} else {
				ret = new token(T_COLON, ":", _file, _line, _column-1);
				return ret;
			}
		}
		if (ch == '{') {
			ret = new token(T_LBRACE, "{", _file, _line, _column);
			NEXT;
			return ret;
		}
		if (ch == '}') {
			ret = new token(T_RBRACE, "}", _file, _line, _column);
			NEXT;
			return ret;
		}
		if (ch == '(') {
			ret = new token(T_LPAREN, "(", _file, _line, _column);
			NEXT;
			return ret;
		}
		if (ch == ')') {
			ret = new token(T_RPAREN, ")", _file, _line, _column);
			NEXT;
			return ret;
		}
		if (ch == ';') {
			ret = new token(T_SEMICOLON, ";", _file, _line, _column);
			NEXT;
			return ret;
		}
		if (ch == '+') {
			ret = new token(T_PLUS, "+", _file, _line, _column);
			NEXT;
			return ret;
		}
		if (ch == '-') {
			NEXT;
			if (ch == '>') {
				NEXT;
				ret = new token(T_ARROW, "->", _file, _line, _column-2);
			} else {
				ret = new token(T_MINUS, "-", _file, _line, _column-1);
			}
			return ret;
		}
		if (ch == '*') {
			ret = new token(T_MUL, "*", _file, _line, _column);
			NEXT;
			return ret;
		}
		if (ch == '/') {
			ret = new token(T_DIV, "/", _file, _line, _column);
			NEXT;
			return ret;
		}
		if (ch == '=') {
			ret = new token(T_ASSIGN, "=", _file, _line, _column);
			NEXT;
			return ret;
		}
		if (ch == ',') {
			ret = new token(T_COMMA, ",", _file, _line, _column);
			NEXT;
			return ret;
		}

		if (ch == '"') {
			return read_string(ch);
		}

		if (ch == '.') {
			if (isdigit(PEEK)) {
				ret = read_number(ch);
			} else if (PEEK == '.') {
				NEXT;
				NEXT;
				ret = new token(T_DOTDOT, "..", _file, _line, _column - 2);
			} else {
				NEXT;
				ret = new token(T_DOT, ".", _file, _line, _column - 1);
			}
			return ret;
		}

		if (isdigit(ch)) {
			return read_number(ch);
		}

		return read_ident(ch);
	}
	return nullptr;
}

// TODO: I want """-""" strings in this language
token* lexer_state::read_string(int ch)
{
	if (ch != '"') {
		LOG_ERROR("err, bug in compiler");
		return nullptr;
	}

	int start_col = _column;

	string text;
	NEXT;
	while (ch != '"') {
		if (ch == '\\') {
			NEXT;
			switch (ch) {
			case '\\':
			case '"':
				text += ch;
				NEXT;
				continue;
			case 'n':
				text += '\n';
				NEXT;
				continue;
			case 'r':
				text += '\r';
				NEXT;
				continue;
			case 'b':
				text += '\b';
				NEXT;
				continue;
			case 't':
				text += '\t';
				NEXT;
				continue;
			default:
				LOG_WARN("Unknown escape sequence in string");
				NEXT;
				continue;
			}
		}
		if (ch == '\n') {
			LOG_ERROR("err, newline in string");
			return nullptr;
		}
		text += ch;
		NEXT;
	}
	NEXT; // skip "
	return new token(T_STRING, text, _file, _line, start_col);
}

token* lexer_state::read_ident(int ch)
{
	char buf[1024];
	int n = 1;
	if (!isident0(ch)) {
		LOG_ERROR("Unexpected '%c'", ch);
		return nullptr;
	}
	buf[0] = ch;
	while (isidentn(PEEK)) {
		NEXT;
		buf[n++] = ch;
		if (n == 1024) {
			LOG_ERROR("identifier too long");
			return nullptr;
		}
	}
	NEXT;
	buf[n] = '\0';
	if (keywords.find(buf) != keywords.end())
		return new token(T_KEYWORD, buf, _file, _line, _column - n);
	else
		return new token(T_IDENTIFIER, buf, _file, _line, _column - n);
}

token* lexer_state::read_number(int ch)
{
	char buf[512];
	int n = 1;
	buf[0] = ch;
	while (isnumeric(PEEK)) {
		NEXT;
		buf[n++] = ch;
		if (n == 512) {
			LOG_ERROR("number constant too big");
			return nullptr;
		}
	}
	NEXT;
	buf[n] = '\0';

	if (strchr(buf, '.')) {
		return new token(T_FLOAT, buf, _file, _line, _column - n);
	} else {
		return new token(T_INT, buf, _file, _line, _column - n);
	}
}
