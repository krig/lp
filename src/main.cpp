#include "common.hpp"
#include "os.hpp"
#include "lexer.hpp"

int main(int argc, char* argv[]) {
	if (argc < 2) {
		LOG_ERROR("Expected <file>");
		return 1;
	}

	const char* infile = argv[argc-1];
	if (!os::path::exists(infile)) {
		LOG_ERROR("File not found: %s", infile);
		return 1;
	}

	token* t;
	lexer_state state;
	state.init(infile);
	while ((t = state.next_token()) != nullptr) {
		LOG_INFO("%s (%s)", t->name(), t->text());
		delete t;
	}

	return 0;
}
