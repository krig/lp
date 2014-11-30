#include "common.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include "module.hpp"

void parser_state::init(lexer_state *mainfile)
{
	_mainfile = mainfile;
}

module *parser_state::parse()
{
	printf("parsing...\n");
	module* m = new module("main");
	m->_file = _mainfile->_file;
	token t = _mainfile->next_token();
	{ auto desc = t.to_str(); LOG_INFO("%s", desc.c_str()); }
	switch (t._type) {
	case T_KEYWORD: {
		printf("keyword %s\n", t._text);
		if (strcmp(t._text, "def") == 0) {
			printf("def!\n");
		}
	} break;
	case T_IDENTIFIER: {
		token op = _mainfile->next_token();
		if (op._type == T_DOUBLE_COLON) {
			// immutable definition
		} else if (op._type == T_COLON_ASSIGN) {
			// mutable definition
		} else if (op._type == T_COLON) {
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
