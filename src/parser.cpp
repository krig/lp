#include "common.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include "module.hpp"

struct cursor {
	parser_state* state;
	int idx;

	explicit cursor(parser_state* state) : state(state), idx(-1) {}
	cursor(parser_state* state, int idx) : state(state), idx(idx) {}
	cursor(const cursor& c) : state(c.state), idx(c.idx) {}
	cursor& operator=(const cursor& c) {
		if (this == &c)
			return *this;
		state = c.state;
		idx = c.idx;
		return *this;
	}

	const token& get() {
		while (state->history.size() <= idx)
			state->history.push_back(state->lexer->next_token());
		return state->history[idx];
	}

	const token& peek() {
		while (state->history.size() <= idx + 1)
			state->history.push_back(state->lexer->next_token());
		return state->history[idx + 1];
	}

	void consume() {
		++idx;
	}
};

bool match_keyword(cursor& c, const char* kw)
{
	const token& t = c.peek();
	return t.type == T_KEYWORD && t.text == kw;
}

bool match_token(cursor& c, Token t)
{
	return c.peek().type == t;
}

bool parse_def(cursor& c)
{
	token ident = c.peek();
	c.consume();
	if (match_token(c, T_LPAREN))
		c.consume();
	return true;
}

bool parse_toplevel(parser_state* state, module* m)
{
	cursor c(state);
	if (c.peek().type == T_EOF)
		return false;
	if (match_keyword(c, "def")) {
		c.consume();
		return parse_def(c);
	}
	return false;
}

void parser_state::init(lexer_state* lexer)
{
	this->lexer = lexer;
	this->history.clear();
}


module *parser_state::parse()
{
	printf("parsing...\n");
	module* m = new module("main");
	m->_files.push_back(lexer->_file);

	while (parse_toplevel(this, m))
		;

	return m;
}

void parser_state::error(const char* fmt, ...)
{
	char buf[2048];
	va_list va_args;
	va_start(va_args, fmt);
	vsnprintf(buf, 2048, fmt, va_args);
	va_end(va_args);

	const char* f;
	int l;
	f = lexer->_file;
	l = lexer->_stream._line;

	logging::log_context(f, l, "").error("Parser error: %s", buf);
}

void parser_state::warning(const char* fmt, ...)
{
	char buf[2048];
	va_list va_args;
	va_start(va_args, fmt);
	vsnprintf(buf, 2048, fmt, va_args);
	va_end(va_args);

	const char* f;
	int l;
	f = lexer->_file;
	l = lexer->_stream._line;

	logging::log_context(f, l, "").warn("Parser warning: %s", buf);
}
