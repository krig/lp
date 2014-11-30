#pragma once

#include "lexer.hpp"
#include "parser.hpp"

struct module : ast::unit {
	explicit module(const char* module_id);

	string _id;
	string _file;
};

typedef dict<string, module *> module_registry;

module* get(module_registry *r, const char* module_id);


