#pragma once

struct lexer_state;
struct module;

struct parser_state {

	void init(lexer_state *mainfile);

	module *parse();

	lexer_state *_mainfile;
};
