#include "common.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include "module.hpp"

struct ast_expr;
struct parse_error;

struct parser_result {
	enum result_type {
		Error,
		Mismatch,
		Expr
	} type;
	union {
		ast_expr* _expr;
		parse_error* _error;
	};

	bool mismatch() const { return type == Mismatch; }
	ast_expr* expr() const { return (type == Expr) ? _expr : nullptr; }
	parse_error* error() const { return (type == Error) ? _error : nullptr; }

	parser_result() : type(Mismatch), _expr(nullptr) {}
	explicit parser_result(ast_expr* expr) : type(Expr), _expr(expr) {}
	explicit parser_result(parse_error* error) : type(Error), _error(error) {}
};

struct parse_error {
	virtual ~parse_error() {}
	virtual const char* what() = 0;
};

typedef parser_result ParseFunction(parser_state* state);

parser_result parse_toplevel(parser_state* state)
{
	return parser_result();
}


void parser_state::init(lexer_state* lexer)
{
	this->lexer = lexer;
}


module *parser_state::parse()
{
	printf("parsing...\n");
	module* m = new module("main");
	m->_files.push_back(lexer->_file);

	while (true) {
		auto result = parse_toplevel(this);
		if (result.error()) {
			error("Parse error: %s", result.error()->what());
		} else if (result.expr()) {
			// TODO: handle successful parse
		} else {
			// TODO
			break;
		}
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
