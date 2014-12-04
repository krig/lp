#pragma once

#include <deque>

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
		ident* _type;
	};

	struct leaf : node {
	};

	struct ident : leaf {
		symbol text;
	};

	struct vardef : stmt {
		ident* _name;
		ident* _type;
		expr* _expr;
		bool _mutable;
	};

	struct arglist : leaf {
		//vector<> args;
	};

	struct retlist : leaf {
		//vector<> rets;
	};

	struct fundecl : expr {
		arglist* _arglist;
		retlist* _retlist;
		block* _block;
	};

	struct fundef : stmt {
		ident* _name;
		fundecl* _fundecl;
	};

	struct block : expr {
		capture* _capture;
		vector<stmt*> _body;
	};

 	struct unit : node {
		vector<stmt*> stmts;
	};

}

struct parser_state {

	void init(lexer_state *mainfile);

	module *parse();

	unique_ptr<ast::ident> parse_ident();
	unique_ptr<ast::arglist> parse_arglist();
	unique_ptr<ast::retlist> parse_retlist();
	unique_ptr<ast::block> parse_block();
	unique_ptr<ast::fundecl> parse_fundecl();
	unique_ptr<ast::fundef> parse_fundef();

	bool advance();
	token get_token(int n);

	bool match(Token type);
	bool match_keyword(const char* kw);

	void error(const char* fmt, ...);
	void warning(const char* fmt, ...);

	lexer_state *_mainfile;
	module *_module;

	std::deque<token> _tokens;
};
