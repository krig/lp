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
	token t = _mainfile->next_token();
	switch (t._type) {
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
		string ts = t.to_str();
		LOG_ERROR("Unexpected token: %s", ts.c_str());
		return nullptr;
	};
	}
	return nullptr;
}
