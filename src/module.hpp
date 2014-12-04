#pragma once

#include "lexer.hpp"
#include "parser.hpp"

struct type_declaration;
struct value_expression;

struct declaration {
	string name;
	type_declaration* type;
	value_expression* expr;
};

struct module {
	explicit module(const char* id) : _id(id) {}

	string _id;

	dict<string, string> _env;
	vector<string> _deps;
	vector<string> _files;
	vector<declaration*> _decls;
};

struct program {
	module* _builtins;
	vector<module*> _modules;
	module* _main;
};
