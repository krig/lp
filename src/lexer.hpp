#pragma once

#include "strfmt.hpp"
#include "intern.hpp"
#include <deque>


#define TOKENS					\
	TOKEN(EOF),				\
		TOKEN(KEYWORD),			\
		TOKEN(IDENTIFIER),		\
		TOKEN(INT),			\
		TOKEN(FLOAT),			\
		TOKEN(STRING),			\
		TOKEN(INTERP_STRING),		\
		TOKEN(DOUBLE_COLON),		\
		TOKEN(SEMICOLON),		\
		TOKEN(LPAREN),			\
		TOKEN(RPAREN),			\
		TOKEN(LBRACE),			\
		TOKEN(RBRACE),			\
		TOKEN(LBRACKET),		\
		TOKEN(RBRACKET),		\
		TOKEN(PLUS),			\
		TOKEN(MINUS),			\
		TOKEN(MUL),			\
		TOKEN(DIV),			\
		TOKEN(LT),			\
		TOKEN(GT),			\
		TOKEN(GTEQ),			\
		TOKEN(LTEQ),			\
		TOKEN(LSHIFT),			\
		TOKEN(RSHIFT),			\
		TOKEN(ASSIGN),			\
		TOKEN(COLON_ASSIGN),		\
		TOKEN(COLON),			\
		TOKEN(ARROW),			\
		TOKEN(DOT),			\
		TOKEN(DOTDOT),			\
		TOKEN(DOTDOTDOT),		\
		TOKEN(COMMA),			\
		TOKEN(EXCLAMATION),		\
		TOKEN(QUESTION),		\
		TOKEN(AT),			\
		TOKEN(EQUALS),			\
		TOKEN(NOT_EQUALS),		\
		TOKEN(AND),			\
		TOKEN(VERTICAL_BAR),		\
		TOKEN(DOUBLE_AND),		\
		TOKEN(DOUBLE_OR),		\

enum Token {
#define TOKEN(t) T_##t
	TOKENS
#undef TOKEN
};

extern const char* token_name[];

typedef uint32_t rune_t;

struct token {
	token() {}
	token(Token type, const char* text, const char* file, int line, int column)
		: type(type), text(text), _file(file), _line(line), _column(column) {}
	token(const token& token) : type(token.type), text(token.text), _file(token._file),
				    _line(token._line), _column(token._column) {}

	token& operator=(const token& token) {
		if (&token == this)
			return *this;
		type = token.type;
		text = token.text;
		_file = token._file;
		_line = token._line;
		_column = token._column;
		return *this;
	}

	const char* name() const { return token_name[type]; }

	strfmt<> to_str() const {
		return strfmt<>("%s (%s:%d:%d) [%s]",
				token_name[type],
				_file, _line, _column,
				text());
	}

	Token type;
	symbol text;
	const char* _file;
	int _line;
	int _column;
};


struct u8_stream {
	u8_stream();
	rune_t get();
	rune_t peek();
	rune_t advance();
	bool eof() const;
	void read_file(const char* filename);

	string _text;
	int _offset;
	int _line;
	int _column;

	rune_t _curr;
	rune_t _peek;
	int _peek_offs;
	std::deque<int> _unget;
};


struct lexer_state {
	lexer_state();
	void init(const char* filename);
	token next_token();
	token* next_token_impl();
	token* read_string(int ch);
	token* read_ident(int ch);
	token* read_number(int ch);
	void error(const char* fmt, ...);
	void warning(const char* fmt, ...);

	const char* _file;
	u8_stream _stream;
	token _token;
};

