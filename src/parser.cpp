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
		error("Expected identifier, got %s", _curr.to_str().c_str());
		throw ::error("Expected identifier, got %s", _curr.to_str().c_str());
	}
	unique_ptr<ast::ident> id(new ast::ident);
	id->text = _prev.text;
	return id;
}

unique_ptr<ast::arglist> parser_state::parse_arglist()
{
	match(T_LPAREN);
	match(T_RPAREN);
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
	next_token();
	auto id = parse_ident();
	auto fd = parse_fundecl();
	fundef->_name = id.release();
	fundef->_fundecl = fd.release();
	printf("fundef: %s\n", fundef->_name->text());
	return fundef;
}

void parser_state::next_token()
{
	_prev = _curr;
	_curr = _mainfile->next_token();
}

bool parser_state::match(Token type)
{
	if (_curr.type == type) {
		next_token();
		return true;
	}
	return false;
}

bool parser_state::match_keyword(const char* kw)
{
	if (_curr.type == T_KEYWORD && _curr.text == kw) {
		next_token();
		return true;
	}
	return false;
}

module *parser_state::parse()
{
	printf("parsing...\n");
	module* m = new module("main");
	m->_file = _mainfile->_file;
	token t = _mainfile->next_token();
	{ auto desc = t.to_str(); LOG_INFO("%s", desc.c_str()); }
	switch (t.type) {
	case T_KEYWORD: {
		if (t.text == "def") {
			auto v = parse_fundef();
			if (v)
				m->stmts.push_back(v.release());
		} else if (t.text == "use") {
		}
	} break;
	case T_IDENTIFIER: {
		token op = _mainfile->next_token();
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
	return m;
}

void parser_state::error(const char* fmt, ...)
{
	char buf[2048];
	va_list va_args;
	va_start(va_args, fmt);
	vsnprintf(buf, 2048, fmt, va_args);
	va_end(va_args);

	logging::log_context(_mainfile->_file, _mainfile->_stream._line, "").error("Parser error: %s", buf);
}

void parser_state::warning(const char* fmt, ...)
{
	char buf[2048];
	va_list va_args;
	va_start(va_args, fmt);
	vsnprintf(buf, 2048, fmt, va_args);
	va_end(va_args);

	logging::log_context(_mainfile->_file, _mainfile->_stream._line, "").warn("Parser warning: %s", buf);
}
