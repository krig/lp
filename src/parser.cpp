#include "common.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include "module.hpp"

void parser_state::init(lexer_state *mainfile)
{
	_mainfile = mainfile;
}

unique_ptr<ast::ident> parser_state::parse_ident()
{
	if (!match(T_IDENTIFIER)) {
		error("Expected identifier, got %s", get_token(0).to_str().c_str());
		throw ::error("Expected identifier, got %s", get_token(0).to_str().c_str());
	}
	unique_ptr<ast::ident> id(new ast::ident);
	id->text = get_token(0).text;
	advance();
	return id;
}

unique_ptr<ast::arglist> parser_state::parse_arglist()
{
	if (!match(T_LPAREN))
		return unique_ptr<ast::arglist>();
	advance();

	//vector<argument*> args;

	if (!match(T_RPAREN))
		return unique_ptr<ast::arglist>();
	return unique_ptr<ast::arglist>();
}

unique_ptr<ast::retlist> parser_state::parse_retlist()
{
	match(T_LPAREN);
	match(T_RPAREN);
	return unique_ptr<ast::retlist>();
}

unique_ptr<ast::block> parser_state::parse_block()
{
	match(T_LBRACE);
	match(T_RBRACE);
	return unique_ptr<ast::block>();
}

unique_ptr<ast::fundecl> parser_state::parse_fundecl()
{
	auto al = parse_arglist();
	if (match(T_ARROW)) {
		auto rl = parse_retlist();
	}
	auto bl = parse_block();
	return unique_ptr<ast::fundecl>();
}

unique_ptr<ast::fundef> parser_state::parse_fundef()
{
	unique_ptr<ast::fundef> fundef(new ast::fundef);
	advance();
	auto id = parse_ident();
	auto fd = parse_fundecl();
	fundef->_name = id.release();
	fundef->_fundecl = fd.release();
	printf("fundef: %s\n", fundef->_name->text());
	return fundef;
}

bool parser_state::advance()
{
	if (_tokens.size() > 0)
		_tokens.pop_front();
	if (_tokens.size() == 0) {
		token t = _mainfile->next_token();
		if (t.type != T_EOF)
			_tokens.push_back(t);
	}
	if (_tokens.size() > 0) {
		auto desc = get_token(0).to_str();
		LOG_INFO("%s", desc.c_str());
	}
	return _tokens.size() > 0;
}

token parser_state::get_token(int n) {
	while (n > _tokens.size())
		_tokens.push_back(_mainfile->next_token());
	return _tokens.at(n);
}


bool parser_state::match(Token type)
{
	if (get_token(0).type == type) {
		return true;
	}
	return false;
}

bool parser_state::match_keyword(const char* kw)
{
	token t = get_token(0);
	if (t.type == T_KEYWORD && t.text == kw) {
		return true;
	}
	return false;
}

module *parser_state::parse()
{
	printf("parsing...\n");
	module* m = new module("main");
	m->_files.push_back(_mainfile->_file);

	// AST building:
	// while there are more tokens
	// match a toplevel statement;
	//
	// from AST build in-memory program representation:
	// while incomplete;
	// generate information about functions and types
	// evaulate compile-time expressions

	/*
	token t = get_token(0);
	switch (t.type) {
	case T_EOF:
		return m;
	case T_KEYWORD: {
		if (t.text == "def") {
			auto v = parse_fundef();
			if (v)
				m->stmts.push_back(v.release());
		} else if (t.text == "use") {
		}
	} break;
	case T_IDENTIFIER: {
		advance();
		token op = get_token(0);
		if (op.type == T_DOUBLE_COLON) {
			// immutable definition
		} else if (op.type == T_COLON_ASSIGN) {
			// mutable definition
		} else if (op.type == T_COLON) {
			// typed mutable definition
		}
	} break;
	default: {
		LOG_ERROR("Unexpected token: %s", t.to_str().c_str());
		return nullptr;
	};
	}
	*/
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
	if (_tokens.size() > 0) {
		f = _tokens.front()._file;
		l = _tokens.front()._line;
	} else {
		f = _mainfile->_file;
		l = _mainfile->_stream._line;
	}

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
	if (_tokens.size() > 0) {
		f = _tokens.front()._file;
		l = _tokens.front()._line;
	} else {
		f = _mainfile->_file;
		l = _mainfile->_stream._line;
	}

	logging::log_context(f, l, "").warn("Parser warning: %s", buf);
}
