#include "common.hpp"
#include "os.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include "compile.hpp"
#include "module.hpp"

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

	token t;
	lexer_state lex;
	lex.init(infile);

	parser_state parse;
	parse.init(&lex);

	module* m = parse.parse();

	compile(m);

	#if 0
	while ((t = lex.next_token())._type != T_EOF) {
		string desc = t.to_str();
		LOG_INFO("%s", desc.c_str());
	}
	#endif

	return 0;
}
