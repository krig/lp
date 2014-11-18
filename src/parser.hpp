#pragma once

struct lexer_state;
struct module;

namespace ast {
	struct node;
	struct def;
	struct fundecl;
	struct block;
	struct capture;
	struct stmt;
	struct ident;
	struct expr;

	struct type_information {
	};

	struct node {
	};

	struct stmt : node {
	};

	struct expr : node {
		ident *_type;
	};

	struct leaf : node {
	};

	struct ident : leaf {
	};

	struct def : stmt {
		ident *_name;
		ident *_type;
		expr *_expr;
		bool _mutable;
	};

	struct fundecl : expr {
		node *_arglist;
		node *_retlist;
		block *_block;
	};

	struct block : expr {
		capture *_capture;
		vector<stmt *> _body;
	};
}

struct parser_state {

	void init(lexer_state *mainfile);

	module *parse();

	lexer_state *_mainfile;
	ast::node *_root;
};
