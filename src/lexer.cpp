#include "common.hpp"
#include "file.hpp"
#include "lexer.hpp"
#include "u8.hpp"

const char* keyword_list[] = {
	"cdef", "def", "return", "if", "then", "else", "elif", "for",
	"match", "use", "struct", "enum", "union", "sizeof",
	"addressof", "type", "with", "and", "or", "case", "in",
	"not", "as"
};


const char* token_name[] = {
#define TOKEN(t) #t
	TOKENS
#undef TOKEN
};

lexer_state::lexer_state() {
}

namespace {
	set<const char*> keywords;

	inline bool isident0(rune_t ch) {
		return isalpha(ch) || ch == '_' || ch > 127;
	}

	inline bool isidentn(rune_t ch) {
		return isalnum(ch) || ch == '_' || ch > 127;
	}

	inline bool isnumeric(rune_t ch) {
		return isdigit(ch) || ch == '_' || ch == '.'
			|| ch == 'o' || ch == 'O'
			|| ch == 'b' || ch == 'B'
			|| ch == 'x' || ch == 'X'
			|| ch == 'u' || ch == 'U'
			|| ch == 'f' || ch == 'F'
			|| ch == 'g' || ch == 'G'
			|| ch == 'e' || ch == 'E';
	}

	int _u8_getter(void * up)
	{
		u8_stream *s = (u8_stream *)up;
		if (s->_unget.size()) {
			int r = s->_unget.front();
			s->_unget.pop_front();
			return r;
		} else if (s->_offset > s->_text.size()) {
			return -1;
		} else {
			return (uint8_t)(s->_text[s->_offset++]);
		}
	}

	void _u8_ungetter(int ch, void *up)
	{
		u8_stream *s = (u8_stream *)up;
		s->_unget.push_back(ch);
	}
}

u8_stream::u8_stream() :
	_text(),
	_offset(0),
	_line(1),
	_column(0),
	_curr(invalid_rune),
	_peek(invalid_rune),
	_peek_offs(0),
	_unget() {
}

rune_t u8_stream::get() {
	if (_curr == invalid_rune)
		advance();
	return _curr;
}

rune_t u8_stream::peek() {
	if (_peek == invalid_rune) {
		int offs = _offset;
		_peek = u8_getc(_u8_getter, _u8_ungetter, this);
		_peek_offs = _offset - offs;
	}
	return _peek;
}

rune_t u8_stream::advance() {
	int prev = _curr;
	int offs = _offset;
	if (_peek != invalid_rune) {
		_curr = _peek;
		_peek = invalid_rune;
		offs = _peek_offs;
	} else {
		_curr = u8_getc(_u8_getter, _u8_ungetter, this);
		offs = _offset - offs;
	}
	if (offs > 0) {
		if (prev == '\n') {
			_line++;
			_column = 0;
		} else {
			_column += 1;
		}
	}
	return _curr;
}

bool u8_stream::eof() const
{
	return _offset >= _text.size();
}

void u8_stream::read_file(const char* filename)
{
	_offset = 0;
	_line = 1;
	_column = 0;
	_text = ::read_file(filename);

}


void lexer_state::init(const char* filename)
{
	if (keywords.size() == 0) {
		for (int i = 0; i < ASIZE(keyword_list); ++i) {
			keywords.insert(intern_string(keyword_list[i], false));
		}
	}

	_file = filename;
	_stream.read_file(filename);
}


	void error(const char* fmt, ...);
	void warning(const char* fmt, ...);


token lexer_state::next_token()
{
	token *t = next_token_impl();
	if (t != nullptr)
		return std::move(*t);
	token eof(T_EOF, intern_string("", false), _file, _stream._line, _stream._column);
	return std::move(eof);
}

void lexer_state::error(const char* fmt, ...)
{
	char buf[2048];
	va_list va_args;
	va_start(va_args, fmt);
	vsnprintf(buf, 2048, fmt, va_args);
	va_end(va_args);

	logging::log_context(_file, _stream._line, "").error("Lexer error: %s", buf);
}

void lexer_state::warning(const char* fmt, ...)
{
	char buf[2048];
	va_list va_args;
	va_start(va_args, fmt);
	vsnprintf(buf, 2048, fmt, va_args);
	va_end(va_args);

	logging::log_context(_file, _stream._line, "").warn("Lexer warning: %s", buf);
}


token* lexer_state::next_token_impl()
{
	int ch;
	token* ret = &_token;

	while (!_stream.eof()) {
		ch = _stream.get();
		if (ch == '\n') {
			ch = _stream.advance();
			continue;
		}
		if (isspace(ch)) {
			ch = _stream.advance();
			continue;
		}
		if (ch == '#') {
			while (ch != '\n' && !_stream.eof()) {
				ch = _stream.advance();
			}
			if (ch == '\n') {
				ch = _stream.advance();
				continue;
			}
		}
		if (ch == ':') {
			ch = _stream.advance();
			if (ch == ':') {
				*ret = token(T_DOUBLE_COLON, intern_string("::", false), _file, _stream._line, _stream._column-1);
				ch = _stream.advance();
				return ret;
			} else if (ch == '=') {
				*ret = token(T_COLON_ASSIGN, intern_string(":=", false), _file, _stream._line, _stream._column-1);
				ch = _stream.advance();
				return ret;
			} else {
				*ret = token(T_COLON, intern_string(":", false), _file, _stream._line, _stream._column-1);
				return ret;
			}
		}
		if (ch == '{') {
			*ret = token(T_LBRACE, intern_string("{", false), _file, _stream._line, _stream._column);
			ch = _stream.advance();
			return ret;
		}
		if (ch == '}') {
			*ret = token(T_RBRACE, intern_string("}", false), _file, _stream._line, _stream._column);
			ch = _stream.advance();
			return ret;
		}
		if (ch == '(') {
			*ret = token(T_LPAREN, intern_string("(", false), _file, _stream._line, _stream._column);
			ch = _stream.advance();
			return ret;
		}
		if (ch == ')') {
			*ret = token(T_RPAREN, intern_string(")", false), _file, _stream._line, _stream._column);
			ch = _stream.advance();
			return ret;
		}
		if (ch == '[') {
			*ret = token(T_LBRACKET, intern_string("[", false), _file, _stream._line, _stream._column);
			ch = _stream.advance();
			return ret;
		}
		if (ch == ']') {
			*ret = token(T_RBRACKET, intern_string("]", false), _file, _stream._line, _stream._column);
			ch = _stream.advance();
			return ret;
		}
		if (ch == ';') {
			*ret = token(T_SEMICOLON, intern_string(";", false), _file, _stream._line, _stream._column);
			ch = _stream.advance();
			return ret;
		}
		if (ch == '+') {
			*ret = token(T_PLUS, intern_string("+", false), _file, _stream._line, _stream._column);
			ch = _stream.advance();
			return ret;
		}
		if (ch == '>') {
			ch = _stream.advance();
			if (ch == '>') {
				ch = _stream.advance();
				*ret = token(T_RSHIFT, intern_string(">>", false), _file, _stream._line, _stream._column-2);
			} else if (ch == '=') {
				ch = _stream.advance();
				*ret = token(T_GTEQ, intern_string(">=", false), _file, _stream._line, _stream._column-2);
			} else {
				*ret = token(T_GT, intern_string(">", false), _file, _stream._line, _stream._column-1);
			}
			return ret;
		}
		if (ch == '<') {
			ch = _stream.advance();
			if (ch == '<') {
				ch = _stream.advance();
				*ret = token(T_LSHIFT, intern_string("<<", false), _file, _stream._line, _stream._column-2);
			} else if (ch == '=') {
				ch = _stream.advance();
				*ret = token(T_LTEQ, intern_string("<=", false), _file, _stream._line, _stream._column-2);
			} else {
				*ret = token(T_LT, intern_string("<", false), _file, _stream._line, _stream._column-1);
			}
			return ret;
		}
		if (ch == '-') {
			ch = _stream.advance();
			if (ch == '>') {
				ch = _stream.advance();
				*ret = token(T_ARROW, intern_string("->", false), _file, _stream._line, _stream._column-2);
			} else {
				*ret = token(T_MINUS, intern_string("-", false), _file, _stream._line, _stream._column-1);
			}
			return ret;
		}
		if (ch == '*') {
			*ret = token(T_MUL, intern_string("*", false), _file, _stream._line, _stream._column);
			ch = _stream.advance();
			return ret;
		}
		if (ch == '/') {
			*ret = token(T_DIV, intern_string("/", false), _file, _stream._line, _stream._column);
			ch = _stream.advance();
			return ret;
		}
		if (ch == '=') {
			ch = _stream.advance();
			if (ch == '=') {
				ch = _stream.advance();
				*ret = token(T_EQUALS, intern_string("=", false), _file, _stream._line, _stream._column-2);
			} else {
				*ret = token(T_ASSIGN, intern_string("=", false), _file, _stream._line, _stream._column-1);
			}
			return ret;
		}
		if (ch == ',') {
			*ret = token(T_COMMA, intern_string(",", false), _file, _stream._line, _stream._column);
			ch = _stream.advance();
			return ret;
		}
		if (ch == '!') {
			ch = _stream.advance();
			if (ch == '=') {
				ch = _stream.advance();
				*ret = token(T_NOT_EQUALS, intern_string("!=", false), _file, _stream._line, _stream._column-2);
			} else {
				*ret = token(T_EXCLAMATION, intern_string("!", false), _file, _stream._line, _stream._column-1);
			}
			return ret;
		}
		if (ch == '?') {
			*ret = token(T_QUESTION, intern_string("?", false), _file, _stream._line, _stream._column);
			ch = _stream.advance();
			return ret;
		}
		if (ch == '@') {
			*ret = token(T_AT, intern_string("@", false), _file, _stream._line, _stream._column);
			ch = _stream.advance();
			return ret;
		}
		if (ch == '&') {
			ch = _stream.advance();
			if (ch == '&') {
				ch = _stream.advance();
				*ret = token(T_DOUBLE_AND, intern_string("&&", false), _file, _stream._line, _stream._column-2);
			} else {
				*ret = token(T_AND, intern_string("&", false), _file, _stream._line, _stream._column-1);
			}
			return ret;
		}
		if (ch == '|') {
			ch = _stream.advance();
			if (ch == '&') {
				ch = _stream.advance();
				*ret = token(T_DOUBLE_OR, intern_string("||", false), _file, _stream._line, _stream._column-2);
			} else {
				*ret = token(T_VERTICAL_BAR, intern_string("|", false), _file, _stream._line, _stream._column);
			}
			return ret;
		}

		if (ch == '"') {
			return read_string(ch);
		}

		if (ch == '.') {
			if (isdigit(_stream.peek())) {
				ret = read_number(ch);
			} else if (_stream.peek() == '.') {
				ch = _stream.advance();
				if (_stream.peek() == '.') {
					ch = _stream.advance();
					ch = _stream.advance();
					*ret = token(T_DOTDOTDOT, intern_string("...", false), _file, _stream._line, _stream._column - 2);
				} else {
					ch = _stream.advance();
					*ret = token(T_DOTDOT, intern_string("..", false), _file, _stream._line, _stream._column - 2);
				}
			} else {
				ch = _stream.advance();
				*ret = token(T_DOT, intern_string(".", false), _file, _stream._line, _stream._column - 1);
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
		error("Expected \", got %d (0x%x)", ch, ch);
		return nullptr;
	}

	int start_line = _stream._line;
	int start_col = _stream._column;

	string text;
	ch = _stream.advance();
	while (ch != '"') {
		if (ch == '\\') {
			ch = _stream.advance();
			switch (ch) {
			case '\\':
			case '"':
				text += ch;
				break;
			case 'n':
				text += '\n';
				break;
			case 'r':
				text += '\r';
				break;
			case 'b':
				text += '\b';
				break;
			case 't':
				text += '\t';
				break;
			default:
				warning("Unknown escape sequence in string");
				break;
			}
			ch = _stream.advance();
			continue;
		}
		if (ch == '\n') {
			error("err, newline in string");
			return nullptr;
		}
		u8_append(text, ch);
		ch = _stream.advance();
	}
	ch = _stream.advance(); // skip "
	_token = token(T_STRING, intern_string(text.c_str(), true), _file, start_line, start_col);
	return &_token;
}

token* lexer_state::read_ident(int ch)
{
	char buf[1024];
	int ap = 0;
	int start_line = _stream._line;
	int start_col = _stream._column;
	if (!isident0(ch)) {
		error("Unexpected '%c' (%d)", ch, ch);
		return nullptr;
	}
	buf[0] = '\0';
	ap = u8_append(buf, ch, sizeof(buf));
	while (isidentn(_stream.peek())) {
		ch = _stream.advance();
		ap = u8_append(buf, ch, sizeof(buf));
		if (ap >= sizeof(buf)) {
			error("identifier too long");
			return nullptr;
		}
	}
	ch = _stream.advance();
	buf[ap] = '\0';
	if (strcmp(buf, "use") == 0)
		LOG_INFO("use!");
	const char* interned = intern_string(buf, true);
	if (keywords.find(interned) != keywords.end())
		_token = token(T_KEYWORD, interned, _file, start_line, start_col);
	else
		_token = token(T_IDENTIFIER, interned, _file, start_line, start_col);
	return &_token;
}

token* lexer_state::read_number(int ch)
{
	char buf[512];
	int n = 1;
	int start_line = _stream._line;
	int start_col = _stream._column;
	buf[0] = ch;
	while (isnumeric(_stream.peek())) {
		ch = _stream.advance();
		buf[n++] = ch;
		if (n == 512) {
			error("number constant too big");
			return nullptr;
		}
	}
	ch = _stream.advance();
	buf[n] = '\0';

	// TODO: proper number parsing
	if (strchr(buf, '.')) {
		_token = token(T_FLOAT, intern_string(buf, true), _file, start_line, start_col);
	} else {
		_token = token(T_INT, intern_string(buf, true), _file, start_line, start_col);
	}
	return &_token;
}
