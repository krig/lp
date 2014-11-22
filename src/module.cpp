#include "common.hpp"
#include "module.hpp"

module *get(module_registry *r, const char *module_id)
{
	auto i = r->find(module_id);
	if (i != r->end())
		return i->second;
	module *ret = new module(module_id);
	(*r)[module_id] = ret;
	return ret;
}

module::module(const char *module_id) : _id(module_id), _file(module_id)
{
}
