#include "common.hpp"
#include "os.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include "compile.hpp"
#include "module.hpp"
#include "options.hpp"

void compiler_main(const char* infile, const char* outfile) {
	ASSERT2(strcmp(infile, outfile) != 0, "This just isn't a good idea");

	token t;
	lexer_state lex;
	lex.init(infile);

	parser_state parse;
	parse.init(&lex);

	module* m = parse.parse();

	compile(outfile, m);

	#if 0
	while ((t = lex.next_token())._type != T_EOF) {
		string desc = t.to_str();
		LOG_INFO("%s", desc.c_str());
	}
	#endif
}

int main(int argc, char* argv[]) {
	optionparser parser;
	parser("compile").doc("compile");
	parser("compile").add_opt("o", "out-file", option::Value);
	parser("compile").add_arg("in-file");
	if (!parser.parse("compile", argc-1, argv+1)) {
		return 1;
	}

	const char* infile = parser("compile").arg(0).value().c_str();
	if (!os::path::exists(infile)) {
		LOG_ERROR("File not found: %s", infile);
		return 1;
	}

	string outfile;
	if (parser("compile")["out-file"].flag()) {
		outfile = parser("compile")["out-file"].value();
	} else {
		outfile = os::path::replace_extension(infile, ".cpp");
	}

	try {
		compiler_main(infile, outfile.c_str());
	} catch (const error& e) {
		logging::cfprintf(stderr, logging::RED, "%s\n", e.what());
		return 1;
	}

	return 0;
}
