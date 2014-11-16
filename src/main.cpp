#include "common.hpp"
#include "os.hpp"

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

	
	return 0;
}
