#include "common.hpp"
#include "compile.hpp"
#include "module.hpp"

void compile(module *m)
{
	if (m == nullptr)
		return;

	// linearize dependency chain
	// printout all declarations for all modules
	// printout all function bodies for all modules
	// compile
	// link
	printf("// ***module %s\n", m->_id.c_str());
}
