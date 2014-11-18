#include "common.hpp"
#include "parser.hpp"
#include "module.hpp"

// term stack
// operator stack
// handle precedence by pushing / popping the term stack
// so to read the expression 1 + 2 * 5
// push 1 -> terms
// + higher precedence than nil
// push + -> ops
// push 2 -> terms
// * higher precedence than +
// push 5 -> terms
// end of expression
// pop *, 5, 2, push (* 2 5) to terms
// pop +, (* 5 2), 1, push (+ 1 (* 2 5)) to terms
//
// 1 * 2 + 5
// push 1 -> terms
// * higher precedence than nil
// push * -> ops
// push 2 -> terms
// + lower precedence than *
// pop *, 2, 1, push (* 1 2) to terms
// push +
// push 5
// end of expression
// pop +, 5, (* 1 2), push (+ (* 1 2) 5)

void parser_state::init(lexer_state *mainfile)
{
	_mainfile = mainfile;
}

module *parser_state::parse()
{
	
	return nullptr;
}
