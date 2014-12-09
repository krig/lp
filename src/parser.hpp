#pragma once

#include <deque>

struct lexer_state;
struct module;

struct parser_state {
	void init(lexer_state* lexer);
	module *parse();

	void error(const char* fmt, ...);
	void warning(const char* fmt, ...);

	lexer_state* lexer;
};
