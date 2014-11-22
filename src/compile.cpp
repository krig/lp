#include "common.hpp"
#include "compile.hpp"
#include "module.hpp"
#include "file.hpp"

void compile(const string& outfile, module *m)
{
	if (m == nullptr)
		return;

	printf("%s -> %s\n", m->_file.c_str(), outfile.c_str());

	file of(outfile.c_str(), "wb");

	// linearize dependency chain
	// printout all declarations for all modules
	// printout all function bodies for all modules
	// compile
	// link
	of.println("// ***module %s", m->_id.c_str());
}
