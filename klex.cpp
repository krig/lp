#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <vector>

typedef struct
{
	const char* text;
	int32_t type;
	int column;
	int line;
} Token;

enum TokenType
{
	T_EOF = 0,
	T_ERROR = 1,
	T_IDENT = 256,
	T_PLUSPLUS,
	T_MINUSMINUS,
	T_PLUSEQ,
	T_MINUSEQ,
	T_MULMUL,
	T_MULEQ,
	T_DIVEQ,
	T_EQEQ,
	T_GTEQ,
	T_LTEQ,
	T_DBLARROW, // =>
	T_ARROW, // ->
	T_LSHIFT,
	T_RSHIFT,
	T_EXCLAIM, // !
	T_DBLEXCLAIM, // !!
	T_NOTEQ, // !=
	T_DBLCOLON,
	T_COLONEQ,
	T_SEMICOLON,
	T_STRING,
	T_DOTDOT,
	T_DOTDOTDOT,
	T_OROP,
	T_ANDOP,
	T_IF,
	T_ELIF,
	T_ELSE,
	T_FOR,
	T_RETURN,
	T_OR,
	T_AND,
	T_IS,
	T_IN,
	T_NOT,
	T_NUMBER
};

typedef struct
{
	std::vector<char> text;
	std::vector<char*> interned;

	int (*nextfn)();
	int unget;

	int column;
	int line;
} KlexState;

static void append_char(KlexState* state, int ch)
{
	state->text.push_back(ch);
}

static const char* reset_text(KlexState* state)
{
	state->text.clear();
	return "";
}

static char* intern_text(KlexState* state)
{
	state->interned.push_back(strdup(state->text.data()));
	return state->interned.back();
}

static int get_next(KlexState* state)
{
	int ch;
	if (state->unget > -1) {
		ch = state->unget;
		state->unget = -1;
	} else {
		ch = state->nextfn();
		if (ch == '\n') {
			state->column = 0;
			++state->line;
		} else {
			++state->column;
		}
	}
	return ch;
}

static Token parse_string(KlexState* state, int stringtype, int raw)
{
	// """ ... """ multiline strings
	// ''' ... ''' also
	// no syntax for single characters, just use strings..
	Token ret;
	int ch;

	ret.type = T_STRING;
	ret.text = reset_text(state);
	ret.column = state->column;
	ret.line = state->line;

	for (;;) {
		ch = get_next(state);
		if (ch < 0 || ch == '\n') {
			/* TODO: error handling */
		} else if (raw == 0 && ch == '\\') {
			ch = get_next(state);
			switch (ch) {
			case 'n': append_char(state, '\n'); break;
			case 't': append_char(state, '\t'); break;
			case 'r': append_char(state, '\r'); break;
			case 'b': append_char(state, '\b'); break;
			case '\\': append_char(state, '\\'); break;
			case '"': append_char(state, '"'); break;
			case '\'': append_char(state, '\''); break;
			default: /* TODO: error handling */ break;
			}
		} else if (raw == 1 && ch == '\\') {
			append_char(state, '\\');
			ch = get_next(state);
		} else if (ch == stringtype) {
			break;
		}
		append_char(state, ch);
	}

	append_char(state, '\0');
	ret.text = strdup(state->text.data());
	return ret;
}

static Token parse_number(KlexState* state, int initial)
{
	// classify number literal here
	// maybe even normalize here?
	// allow _ in literals?
	// 0x<hex>
	// 0o<octal>
	// 0b<binary>
	// .0f, .0g, 10e7
	Token ret;
	int ch;

	ret.type = T_NUMBER;
	ret.text = reset_text(state);
	ret.column = state->column;
	ret.line = state->line;
	append_char(state, initial);
	if (initial == '0') {
		ch = get_next(state);
		if (ch == 'x' || ch == 'X' ||
		    ch == 'o' || ch == 'O' ||
		    ch == 'b' || ch == 'B')
			append_char(state, ch);
		else
			state->unget = ch;
	}
	for (;;) {
		ch = get_next(state);
		if (ch == '_')
			continue;
		else if (isdigit(ch) || ch == '.' || ch == 'e' || ch == 'E') {
			append_char(state, ch);
		} else if (ch == 'f' || ch == 'F') {
			append_char(state, ch);
			break;
		} else if (ch == 'g' || ch == 'G') {
			append_char(state, ch);
			break;
		} else {
			state->unget = ch;
			break;
		}
	}
	append_char(state, '\0');
	ret.text = intern_text(state);
	return ret;
}

Token klex(KlexState* state)
{
	Token ret;
	int ch, ch2;

restart:
	ch = get_next(state);
	ret.column = state->column;
	ret.line = state->line;
	if (isspace(ch)) goto restart;
	else if (ch == '#') goto skip_comment;
	else if (ch == '+') goto parse_plus;
	else if (ch == '-') goto parse_minus;
	else if (ch == '*') goto parse_mul;
	else if (ch == '/') goto parse_div;
	else if (ch == '=') goto parse_eq;
	else if (ch == '<') goto parse_lt;
	else if (ch == '>') goto parse_gt;
	else if (ch == '!') goto parse_exclaim;
	else if (ch == ':') goto parse_colon;
	else if (ch == '|') goto parse_or;
	else if (ch == '&') goto parse_and;
	else if (ch == ';') { ret.type = ';'; ret.text = ";"; goto done; }
	else if (ch == '(') { ret.type = '('; ret.text = "("; goto done; }
	else if (ch == ')') { ret.type = ')'; ret.text = ")"; goto done; }
	else if (ch == '[') { ret.type = '['; ret.text = "["; goto done; }
	else if (ch == ']') { ret.type = ']'; ret.text = "]"; goto done; }
	else if (ch == '{') { ret.type = '{'; ret.text = "{"; goto done; }
	else if (ch == '}') { ret.type = '}'; ret.text = "}"; goto done; }
	else if (ch == '@') { ret.type = '@'; ret.text = "@"; goto done; }
	else if (ch == '^') { ret.type = '^'; ret.text = "^"; goto done; }
	else if (ch == '%') { ret.type = '%'; ret.text = "%"; goto done; }
	else if (ch == '?') { ret.type = '?'; ret.text = "?"; goto done; }
	else if (ch == ',') { ret.type = ','; ret.text = ","; goto done; }
	else if (ch == '"' || ch == '\'') return parse_string(state, ch, 0);
	else if (ch == 'r' || ch == 'R') goto check_rawstring;
	else if (ch == '.') goto parse_dot;
	else if (isdigit(ch)) return parse_number(state, ch);
	else if (isalpha(ch) || ch == '_') { ret.type = T_IDENT; ret.text = reset_text(state); goto parse_ident_loop; }
	goto eof;

skip_comment:
	if ((ch = get_next(state)) != '\n') goto skip_comment;
	goto restart;

check_rawstring:
	ch2 = get_next(state);
	if (ch2 == '"' || ch2 == '\'') return parse_string(state, ch2, 1);
	state->unget = ch2;
	ret.type = T_IDENT;
	ret.text = reset_text(state);
	goto parse_ident_loop;

parse_plus:
	ch = get_next(state);
	if (ch == '+') { ret.type = T_PLUSPLUS; ret.text = "++"; }
	else if (ch == '=') { ret.type = T_PLUSEQ; ret.text = "+="; }
	else { state->unget = ch; ret.type = '+'; ret.text = "+"; }
	goto done;

parse_minus:
	ch = get_next(state);
	if (ch == '-') { ret.type = T_MINUSMINUS; ret.text = "--"; }
	else if (ch == '=') { ret.type = T_MINUSEQ; ret.text = "-="; }
	else if (ch == '>') { ret.type = T_ARROW; ret.text = "->"; }
	else { state->unget = ch; ret.type = '-'; ret.text = "-"; }
	goto done;

parse_mul:
	ch = get_next(state);
	if (ch == '*') { ret.type = T_MULMUL; ret.text = "**"; }
	else if (ch == '=') { ret.type = T_MULEQ; ret.text = "*="; }
	else { state->unget = ch; ret.type = '*'; ret.text = "*"; }
	goto done;

parse_div:
	ch = get_next(state);
	if (ch == '=') { ret.type = T_DIVEQ; ret.text = "/="; }
	else { state->unget = ch; ret.type = '/'; ret.text = "/"; }
	goto done;

parse_lt:
	ch = get_next(state);
	if (ch == '=') { ret.type = T_LTEQ; ret.text = "<="; }
	else if (ch == '<') { ret.type = T_LSHIFT; ret.text = "<<"; }
	else { state->unget = ch; ret.type = '<'; ret.text = "<"; }
	goto done;

parse_gt:
	ch = get_next(state);
	if (ch == '=') { ret.type = T_GTEQ; ret.text = ">="; }
	else if (ch == '>') { ret.type = T_RSHIFT; ret.text = ">>"; }
	else { state->unget = ch; ret.type = '>'; ret.text = ">"; }
	goto done;

parse_eq:
	ch = get_next(state);
	if (ch == '=') { ret.type = T_EQEQ; ret.text = "=="; }
	else if (ch == '>') { ret.type = T_DBLARROW; ret.text = "=>"; }
	else { state->unget = ch; ret.type = '='; ret.text = "="; }
	goto done;

parse_exclaim:
	ch = get_next(state);
	if (ch == '=') { ret.type = T_NOTEQ; ret.text = "!="; }
	else if (ch == '!') { ret.type = T_DBLEXCLAIM; ret.text = "!!"; }
	else { state->unget = ch; ret.type = '!'; ret.text = "!"; }
	goto done;

parse_colon:
	ch = get_next(state);
	if (ch == '=') { ret.type = T_COLONEQ; ret.text = ":="; }
	else if (ch == ':') { ret.type = T_DBLCOLON; ret.text = "::"; }
	else { state->unget = ch; ret.type = ':'; ret.text = ":"; }
	goto done;

parse_or:
	ch = get_next(state);
	if (ch == '|') { ret.type = T_OROP; ret.text = "||"; }
	else { state->unget = ch; ret.type = '|'; ret.text = "|"; }
	goto done;

parse_and:
	ch = get_next(state);
	if (ch == '&') { ret.type = T_ANDOP; ret.text = "&&"; }
	else { state->unget = ch; ret.type = '&'; ret.text = "&"; }
	goto done;

parse_dot:
	ch = get_next(state);
	if (ch == '.') { ret.type = T_DOTDOT; ret.text = ".."; }
	else if (isdigit(ch)) { state->unget = ch; return parse_number(state, ch); }
	else { state->unget = ch; ret.type = '.'; ret.text = "."; }
	goto done;

parse_ident_loop:
	for (;;) {
		append_char(state, ch);
		ch = get_next(state);
		if (!isalnum(ch) && ch != '_') {
			state->unget = ch;
			append_char(state, '\0');
			ret.text = intern_text(state);
			goto match_keyword;
		}
	}

match_keyword:
	if (strcmp(ret.text, "if") == 0) { ret.type = T_IF; }
	else if (strcmp(ret.text, "elif") == 0) { ret.type = T_ELIF; }
	else if (strcmp(ret.text, "else") == 0) { ret.type = T_ELSE; }
	else if (strcmp(ret.text, "for") == 0) { ret.type = T_FOR; }
	else if (strcmp(ret.text, "return") == 0) { ret.type = T_RETURN; }
	else if (strcmp(ret.text, "or") == 0) { ret.type = T_OR; }
	else if (strcmp(ret.text, "and") == 0) { ret.type = T_AND; }
	else if (strcmp(ret.text, "is") == 0) { ret.type = T_IS; }
	else if (strcmp(ret.text, "in") == 0) { ret.type = T_IN; }
	goto done;

eof:
	ret.type = T_EOF;
	ret.text = "";
done:
	return ret;
}

int main()
{
	KlexState state;
	memset(&state, 0, sizeof(KlexState));
	state.nextfn = getchar;
	state.unget = -1;
	state.line = 1;
	state.column = 0;

	for (;;) {
		auto t = klex(&state);
		if (t.type == T_EOF)
			break;
		printf("%d %s (%d:%d)\n", t.type, t.text, t.line, t.column);
	}

	return 0;
}
