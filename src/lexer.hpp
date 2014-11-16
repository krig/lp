#pragma once

#include "strfmt.hpp"


#define TOKENS					\
	TOKEN(KEYWORD),				\
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

struct token {
	token() {}
	token(Token type, string text, const char* file, int line, int column)
		: _type(type), _text(text), _file(file), _line(line), _column(column) {}
	token(const token& token) : _type(token._type), _text(token._text), _file(token._file),
				    _line(token._line), _column(token._column) {}
	token& operator=(const token& token) {
		if (&token == this)
			return *this;
		_type = token._type;
		_text = token._text;
		_file = token._file;
		_line = token._line;
		_column = token._column;
		return *this;
	}

	const char* name() const { return token_name[_type]; }
	const char* text() const { return _text.c_str(); }

	string to_str() const {
		strfmt<512> s("%s (%s) (%s:%d:%d)",
			      token_name[_type],
			      _text.c_str(),
			      _file, _line, _column);
		return s.str();
	}

	Token _type;
	string _text;
	const char* _file;
	int _line;
	int _column;
};


struct lexer_state {
	lexer_state();
	void init(const char* filename);
	token* next_token();
	token* read_string(int ch);
	token* read_ident(int ch);
	token* read_number(int ch);

	string _text;
	const char* _file;
	int _offset;
	int _line;
	int _column;
	token _token;
};

