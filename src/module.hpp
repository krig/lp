#pragma once

struct module {
	explicit module(const char* module_id);

	string _id;
	vector<string> _use;
};

typedef dict<string, module *> module_registry;

module* get(module_registry *r, const char* module_id);


