/*
 * Copyright (c) 2021 Brian Callahan <bcallah@openbsd.org>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <sys/stat.h>

#include <ctype.h>
#include <fcntl.h>
#include <limits.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define PL0C_VERSION	"1.2.0"

#define CHECK_LHS	0
#define CHECK_RHS	1
#define CHECK_CALL	2

#define TOK_IDENT	'I'
#define TOK_NUMBER	'N'
#define TOK_CONST	'C'
#define TOK_VAR		'V'
#define TOK_PROCEDURE	'P'
#define TOK_CALL	'c'
#define TOK_BEGIN	'B'
#define TOK_END		'E'
#define TOK_IF		'i'
#define TOK_THEN	'T'
#define TOK_ELSE	'e'
#define TOK_WHILE	'W'
#define TOK_DO		'D'
#define TOK_ODD		'O'
#define TOK_WRITEINT	'w'
#define TOK_WRITECHAR	'H'
#define TOK_WRITESTR	'S'
#define TOK_READINT	'R'
#define TOK_READCHAR	'h'
#define TOK_INTO	'n'
#define TOK_SIZE	's'
#define TOK_EXIT	'X'
#define TOK_AND		'&'
#define TOK_OR		'|'
#define TOK_NOT		'~'
#define TOK_FORWARD	'F'
#define TOK_DOT		'.'
#define TOK_EQUAL	'='
#define TOK_COMMA	','
#define TOK_SEMICOLON	';'
#define TOK_ASSIGN	':'
#define TOK_HASH	'#'
#define TOK_LESSTHAN	'<'
#define TOK_GREATERTHAN	'>'
#define TOK_LTEQUALS	'{'
#define TOK_GTEQUALS	'}'
#define TOK_PLUS	'+'
#define TOK_MINUS	'-'
#define TOK_MULTIPLY	'*'
#define TOK_DIVIDE	'/'
#define TOK_MODULO	'%'
#define TOK_LPAREN	'('
#define TOK_RPAREN	')'
#define TOK_LBRACK	'['
#define TOK_RBRACK	']'
#define TOK_STRING	'"'

/*
 * pl0c -- PL/0 compiler.
 *
 * program	= block "." .
 * block	= [ "const" ident "=" number { "," ident "=" number } ";" ]
 *		  [ "var" ident [ array ] { "," ident [ array ] } ";" ]
		  { "forward" ident ";" }
 *		  { "procedure" ident ";" block ";" } statement .
 * statement	= [ ident ":=" expression
 *		  | "call" ident
 *		  | "begin" statement { ";" statement } "end"
 *		  | "if" condition "then" statement [ "else" statement ]
 *		  | "while" condition "do" statement
 *		  | "readInt" [ "into" ] ident
 *		  | "readChar" [ "into" ] ident
 *		  | "writeInt" expression
 *		  | "writeChar" expression
 *		  | "writeStr" ( ident | string )
 *		  | "exit" expression ] .
 * condition	= "odd" expression
 *		| expression ( comparator ) expression .
 * expression	= [ "+" | "-" | "not" ] term { ( "+" | "-" | "or" ) term } .
 * term		= factor { ( "*" | "/" | "mod" | "and" ) factor } .
 * factor	= ident
 *		| number
 *		| "(" expression ")" .
 * comparator	= "=" | "#" | "<" | ">" | "<=" | ">=" | "<>" .
 * array	= "size" number .
 */

static char *raw, *token;
static int depth, proc, type;
static size_t line = 1;

struct symtab {
	int depth;
	int type;
	long size;
	char *name;
	struct symtab *next;
};
static struct symtab *head;

static void expression(void);

/*
 * Misc. functions.
 */

static void
error(const char *fmt, ...)
{
	va_list ap;

	(void) fprintf(stderr, "pl0c: error: %lu: ", line);

	va_start(ap, fmt);
	(void) vfprintf(stderr, fmt, ap);
	va_end(ap);

	(void) fputc('\n', stderr);

	exit(1);
}

static void
readin(char *file)
{
	int fd;
	struct stat st;

	if (strrchr(file, '.') == NULL)
		error("file must end in '.pl0'");

	if (!!strcmp(strrchr(file, '.'), ".pl0"))
		error("file must end in '.pl0'");

	if ((fd = open(file, O_RDONLY)) == -1)
		error("couldn't open %s", file);

	if (fstat(fd, &st) == -1)
		error("couldn't get size");

	if ((raw = malloc(st.st_size + 1)) == NULL)
		error("malloc failed");

	if (read(fd, raw, st.st_size) != st.st_size)
		error("couldn't read %s", file);
	raw[st.st_size] = '\0';

	(void) close(fd);
}

/*
 * Lexer.
 */

static void
comment(void)
{
	int ch;

	while ((ch = *raw++) != '}') {
		if (ch == '\0')
			error("unterminated comment");

		if (ch == '\n')
			++line;
	}
}

static int
ident(void)
{
	char *p;
	size_t i, len;

	p = raw;
	while (isalnum(*raw) || *raw == '_')
		++raw;

	len = raw - p;

	--raw;

	free(token);

	if ((token = malloc(len + 1)) == NULL)
		error("malloc failed");

	for (i = 0; i < len; i++)
		token[i] = *p++;
	token[i] = '\0';

	if (!strcmp(token, "const"))
		return TOK_CONST;
	else if (!strcmp(token, "var"))
		return TOK_VAR;
	else if (!strcmp(token, "procedure"))
		return TOK_PROCEDURE;
	else if (!strcmp(token, "call"))
		return TOK_CALL;
	else if (!strcmp(token, "begin"))
		return TOK_BEGIN;
	else if (!strcmp(token, "end"))
		return TOK_END;
	else if (!strcmp(token, "if"))
		return TOK_IF;
	else if (!strcmp(token, "then"))
		return TOK_THEN;
	else if (!strcmp(token, "else"))
		return TOK_ELSE;
	else if (!strcmp(token, "while"))
		return TOK_WHILE;
	else if (!strcmp(token, "do"))
		return TOK_DO;
	else if (!strcmp(token, "odd"))
		return TOK_ODD;
	else if (!strcmp(token, "writeInt"))
		return TOK_WRITEINT;
	else if (!strcmp(token, "writeChar"))
		return TOK_WRITECHAR;
	else if (!strcmp(token, "writeStr"))
		return TOK_WRITESTR;
	else if (!strcmp(token, "readInt"))
		return TOK_READINT;
	else if (!strcmp(token, "readChar"))
		return TOK_READCHAR;
	else if (!strcmp(token, "into"))
		return TOK_INTO;
	else if (!strcmp(token, "size"))
		return TOK_SIZE;
	else if (!strcmp(token, "exit"))
		return TOK_EXIT;
	else if (!strcmp(token, "and"))
		return TOK_AND;
	else if (!strcmp(token, "or"))
		return TOK_OR;
	else if (!strcmp(token, "not"))
		return TOK_NOT;
	else if (!strcmp(token, "mod"))
		return TOK_MODULO;
	else if (!strcmp(token, "forward"))
		return TOK_FORWARD;

	return TOK_IDENT;
}

static int
number(void)
{
	const char *errstr;
	char *p;
	size_t i, j = 0, len;

	p = raw;
	while (isdigit(*raw) || *raw == '_')
		++raw;

	len = raw - p;

	--raw;

	free(token);

	if ((token = malloc(len + 1)) == NULL)
		error("malloc failed");

	for (i = 0; i < len; i++) {
		if (isdigit(*p))
			token[j++] = *p;
		++p;
	}
	token[j] = '\0';

	(void) strtonum(token, 0, LONG_MAX, &errstr);
	if (errstr != NULL)
		error("invalid number: %s", token);

	return TOK_NUMBER;
}

static int
string(void)
{
	char *p;
	size_t i, len, mod = 0;

	p = ++raw;

restart:
	while (*raw != '\'') {
		if (*raw == '\n' || *raw == '\0')
			error("unterminated string");
		if (*raw++ == '"')
			++mod;
	}
	if (*++raw == '\'') {
		++raw;
		goto restart;
	}

	--raw;

	len = raw - p + mod;

	if (len < 1)
		error("impossible string");

	free(token);

	if ((token = malloc(len + 3)) == NULL)
		error("malloc failed");

	token[0] = '"';
	for (i = 1; i <= len; i++) {
		if (*p == '\'') {
			token[i++] = '\\';
			++p;
		} else if (*p == '"') {
			token[i++] = '\\';
		}
		token[i] = *p++;
	}
	token[i++] = '"';
	token[i] = '\0';

	if (len == 1 || (len == 2 && token[1] == '\\')) {
		token[0] = '\'';
		if (len == 1)
			token[2] = '\'';
		else
			token[3] = '\'';

		return TOK_NUMBER;
	}

	return TOK_STRING;
}

static int
lex(void)
{

again:
	/* Skip whitespace.  */
	while (*raw == ' ' || *raw == '\t' || *raw == '\n') {
		if (*raw++ == '\n')
			++line;
	}

	if (isalpha(*raw) || *raw == '_')
		return ident();

	if (isdigit(*raw))
		return number();

	switch (*raw) {
	case '{':
		comment();
		goto again;
	case '.':
	case '=':
	case ',':
	case ';':
	case '#':
	case '+':
	case '-':
	case '*':
	case '/':
	case '%':
	case '(':
	case ')':
	case '[':
	case ']':
		return (*raw);
	case '<':
		if (*++raw == '=')
			return TOK_LTEQUALS;

		if (*raw == '>')
			return TOK_HASH;

		--raw;

		return TOK_LESSTHAN;
	case '>':
		if (*++raw == '=')
			return TOK_GTEQUALS;

		--raw;

		return TOK_GREATERTHAN;
	case ':':
		if (*++raw != '=')
			error("unknown token: ':%c'", *raw);

		return TOK_ASSIGN;
	case '\'':
		return string();
	case '\0':
		return 0;
	default:
		error("unknown token: '%c'", *raw);
	}

	return 0;
}

/*
 * Code generator.
 */

static void
aout(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	(void) vfprintf(stdout, fmt, ap);
	va_end(ap);
}

static void
cg_array(void)
{

	aout("[%s]", token);
}

static void
cg_call(void)
{

	aout("%s();\n", token);
}

static void
cg_closeblock(void)
{

	aout(";}\n");
}

static void
cg_const(void)
{

	if (proc == 0)
		aout("static ");
	aout("const long %s=", token);
}

static void
cg_crlf(void)
{

	aout("\n");
}

static void
cg_end(void)
{

	aout("/* PL/0 compiler %s */\n", PL0C_VERSION);
}

static void
cg_epilogue(void)
{

	aout(";");

	if (proc == 0)
		aout("\nreturn 0;");

	aout("\n}\n\n");
}

static void
cg_exit(void)
{

	aout("exit(");
}

static void
cg_forward(void)
{

	aout("static void %s(void);", token);
}

static void
cg_init(void)
{

	aout("#include <limits.h>\n");
	aout("#include <stdio.h>\n");
	aout("#include <stdlib.h>\n");
	aout("#include <string.h>\n\n");
	aout("static char __stdin[24];\n");
	aout("static const char *__errstr;\n\n");
	aout("static long __writestridx;\n\n");
}

static void
cg_odd(void)
{

	aout(")&1");
}

static void
cg_procedure(void)
{

	if (proc == 0) {
		aout("int\n");
		aout("main(int argc, char *argv[])\n");
	} else {
		aout("static void\n");
		aout("%s(void)\n", token);
	}

	aout("{\n");
}

static void
cg_readchar(void)
{

	aout("%s=(int) fgetc(stdin);", token);
}

static void
cg_readint(void)
{

	aout("(void) fgets(__stdin, sizeof(__stdin), stdin);\n");
	aout("if(__stdin[strlen(__stdin) - 1] == '\\n')");
	aout("__stdin[strlen(__stdin) - 1] = '\\0';");
	aout("%s=(long) strtonum(__stdin, LONG_MIN, LONG_MAX, &__errstr);\n",
	    token);
	aout("if(__errstr!=NULL){");
	aout("(void) fprintf(stderr, \"invalid number: %%s\\n\", __stdin);");
	aout("exit(1);");
	aout("}");
}

static void
cg_rparen(void)
{

	aout(")");
}

static void
cg_semicolon(void)
{

	aout(";\n");
}

static void
cg_symbol(void)
{

	switch (type) {
	case TOK_IDENT:
	case TOK_NUMBER:
		aout("%s", token);
		break;
	case TOK_BEGIN:
		aout("{\n");
		break;
	case TOK_END:
		aout(";\n}\n");
		break;
	case TOK_IF:
		aout("if(");
		break;
	case TOK_THEN:
		aout("){");
		break;
	case TOK_DO:
		aout(")");
		break;
	case TOK_ELSE:
		aout(";}else{");
		break;
	case TOK_ODD:
		aout("(");
		break;
	case TOK_WHILE:
		aout("while(");
		break;
	case TOK_EQUAL:
		aout("==");
		break;
	case TOK_ASSIGN:
		aout("=");
		break;
	case TOK_HASH:
		aout("!=");
		break;
	case TOK_LTEQUALS:
		aout("<=");
		break;
	case TOK_GTEQUALS:
		aout(">=");
		break;
	default:
		aout("%c", type);
	}
}

static void
cg_var(void)
{

	if (proc == 0)
		aout("static ");
	aout("long %s", token);
}

static void
cg_writechar(void)
{

	aout("(void) fprintf(stdout, \"%%c\", (unsigned char) (");
}

static void
cg_writeint(void)
{

	aout("(void) fprintf(stdout, \"%%ld\", (long) (");
}

static void
cg_writestr(void)
{
	struct symtab *curr, *ret;

	if (type == TOK_IDENT) {
		curr = head;
		while (curr != NULL) {
			if (!strcmp(curr->name, token))
				ret = curr;
			curr = curr->next;
		}

		if (ret == NULL)
			error("undefined symbol: %s", token);

		if (ret->size == 0)
			error("writeStr requires an array");

		aout("__writestridx = 0;\n");
		aout("while(%s[__writestridx]!='\\0'&&__writestridx<%ld)\n",
		    token, ret->size);
		aout("(void)fputc((unsigned char)%s[__writestridx++],stdout);\n",
		    token);
	} else {
		aout("(void)fprintf(stdout, %s);\n", token);
	}
}

/*
 * Semantics.
 */

static void
arraycheck(void)
{
	struct symtab *curr, *ret = NULL;

	curr = head;
	while (curr != NULL) {
		if (!strcmp(token, curr->name))
			ret = curr;
		curr = curr->next;
	}

	if (ret == NULL)
		error("undefined symbol: %s", token);

	if (ret->size == 0)
		error("symbol %s is not an array", token);
}

static void
symcheck(int check)
{
	struct symtab *curr, *ret = NULL;

	curr = head;
	while (curr != NULL) {
		if (!strcmp(token, curr->name))
			ret = curr;
		curr = curr->next;
	}

	if (ret == NULL)
		error("undefined symbol: %s", token);

	switch (check) {
	case CHECK_LHS:
		if (ret->type != TOK_VAR)
			error("must be a variable: %s", token);
		break;
	case CHECK_RHS:
		if (ret->type == TOK_PROCEDURE || ret->type == TOK_FORWARD)
			error("must not be a procedure: %s", token);
		break;
	case CHECK_CALL:
		if (ret->type != TOK_PROCEDURE && ret->type != TOK_FORWARD)
			error("must be a procedure: %s", token);
	}
}

/*
 * Parser.
 */

static void
next(void)
{

	type = lex();
	++raw;
}

static void
expect(int match)
{

	if (match != type)
		error("syntax error");

	next();
}

static void
addsymbol(int type)
{
	struct symtab *curr, *new;

	curr = head;
	while (1) {
		if (!strcmp(curr->name, token)) {
			if (curr->depth == (depth - 1)) {
				if (curr->type == TOK_FORWARD &&
				    type == TOK_PROCEDURE) {
					/* Don't error out!  */
				} else {
					error("duplicate symbol: %s", token);
				}
			}
		}

		if (curr->next == NULL)
			break;

		curr = curr->next;
	}

	if ((new = malloc(sizeof(struct symtab))) == NULL)
		error("malloc failed");

	new->depth = depth - 1;
	new->type = type;
	new->size = 0;
	if ((new->name = strdup(token)) == NULL)
		error("malloc failed");
	new->next = NULL;

	curr->next = new;
}

static void
arraysize(void)
{
	struct symtab *curr;
	const char *errstr;

	curr = head;
	while (curr->next != NULL)
		curr = curr->next;

	if (curr->type != TOK_VAR)
		error("arrays must be declared with \"var\"");

	curr->size = strtonum(token, 1, LONG_MAX, &errstr);
	if (errstr != NULL)
		error("invalid array size");
}

static void
destroysymbols(void)
{
	struct symtab *curr, *prev;

again:
	curr = head;
	while (curr->next != NULL) {
		prev = curr;
		curr = curr->next;
	}

	if (curr->type != TOK_PROCEDURE) {
		free(curr->name);
		free(curr);
		prev->next = NULL;
		goto again;
	}
}

static void
initsymtab(void)
{
	struct symtab *new;

	if ((new = malloc(sizeof(struct symtab))) == NULL)
		error("malloc failed");

	new->depth = 0;
	new->type = TOK_PROCEDURE;
	new->size = 0;
	new->name = "main";
	new->next = NULL;

	head = new;
}

static void
factor(void)
{

	switch (type) {
	case TOK_IDENT:
		symcheck(CHECK_RHS);
		cg_symbol();
		expect(TOK_IDENT);
		if (type == TOK_LBRACK) {
			arraycheck();
			cg_symbol();
			expect(TOK_LBRACK);
			expression();
			if (type == TOK_RBRACK)
				cg_symbol();
			expect(TOK_RBRACK);
		}
		break;
	case TOK_NUMBER:
		cg_symbol();
		expect(TOK_NUMBER);
		break;
	case TOK_LPAREN:
		cg_symbol();
		expect(TOK_LPAREN);
		expression();
		if (type == TOK_RPAREN)
			cg_symbol();
		expect(TOK_RPAREN);
	}
}

static void
term(void)
{

	factor();

	while (type == TOK_MULTIPLY || type == TOK_DIVIDE ||
	    type == TOK_MODULO || type == TOK_AND) {
		cg_symbol();
		next();
		factor();
	}
}

static void
expression(void)
{

	if (type == TOK_PLUS || type == TOK_MINUS || type == TOK_NOT) {
		cg_symbol();
		next();
	}

	term();

	while (type == TOK_PLUS || type == TOK_MINUS || type == TOK_OR) {
		cg_symbol();
		next();
		term();
	}
}

static void
condition(void)
{

	if (type == TOK_ODD) {
		cg_symbol();
		expect(TOK_ODD);
		expression();
		cg_odd();
	} else {
		expression();

		switch (type) {
		case TOK_EQUAL:
		case TOK_HASH:
		case TOK_LESSTHAN:
		case TOK_GREATERTHAN:
		case TOK_LTEQUALS:
		case TOK_GTEQUALS:
			cg_symbol();
			next();
			break;
		default:
			error("invalid conditional");
		}

		expression();
	}
}

static void
statement(void)
{

	switch (type) {
	case TOK_IDENT:
		symcheck(CHECK_LHS);
		cg_symbol();
		expect(TOK_IDENT);
		if (type == TOK_LBRACK) {
			arraycheck();
			cg_symbol();
			expect(TOK_LBRACK);
			expression();
			if (type == TOK_RBRACK)
				cg_symbol();
			expect(TOK_RBRACK);
		}
		if (type == TOK_ASSIGN)
			cg_symbol();
		expect(TOK_ASSIGN);
		expression();
		break;
	case TOK_CALL:
		expect(TOK_CALL);
		if (type == TOK_IDENT) {
			symcheck(CHECK_CALL);
			cg_call();
		}
		expect(TOK_IDENT);
		break;
	case TOK_BEGIN:
		cg_symbol();
		expect(TOK_BEGIN);
		statement();
		while (type == TOK_SEMICOLON) {
			cg_semicolon();
			expect(TOK_SEMICOLON);
			statement();
		}
		if (type == TOK_END)
			cg_symbol();
		expect(TOK_END);
		break;
	case TOK_IF:
		cg_symbol();
		expect(TOK_IF);
		condition();
		if (type == TOK_THEN)
			cg_symbol();
		expect(TOK_THEN);
		statement();
		if (type == TOK_ELSE) {
			cg_symbol();
			expect(TOK_ELSE);
			statement();
		}
		cg_closeblock();
		break;
	case TOK_WHILE:
		cg_symbol();
		expect(TOK_WHILE);
		condition();
		if (type == TOK_DO)
			cg_symbol();
		expect(TOK_DO);
		statement();
		break;
	case TOK_WRITEINT:
		expect(TOK_WRITEINT);
		cg_writeint();
		expression();
		cg_rparen();
		cg_rparen();
		cg_semicolon();

		break;
	case TOK_WRITECHAR:
		expect(TOK_WRITECHAR);
		cg_writechar();
		expression();
		cg_rparen();
		cg_rparen();
		cg_semicolon();

		break;
	case TOK_WRITESTR:
		expect(TOK_WRITESTR);
		if (type == TOK_IDENT || type == TOK_STRING) {
			if (type == TOK_IDENT)
				symcheck(CHECK_LHS);
			cg_writestr();

			if (type == TOK_IDENT)
				expect(TOK_IDENT);
			else
				expect(TOK_STRING);
		} else {
			error("writeStr takes an array or a string");
		}

		break;
	case TOK_READINT:
		expect(TOK_READINT);
		if (type == TOK_INTO)
			expect(TOK_INTO);

		if (type == TOK_IDENT) {
			symcheck(CHECK_LHS);
			cg_readint();
		}

		expect(TOK_IDENT);

		break;
	case TOK_READCHAR:
		expect(TOK_READCHAR);
		if (type == TOK_INTO)
			expect(TOK_INTO);

		if (type == TOK_IDENT) {
			symcheck(CHECK_LHS);
			cg_readchar();
		}

		expect(TOK_IDENT);

		break;
	case TOK_EXIT:
		expect(TOK_EXIT);
		cg_exit();
		expression();
		cg_rparen();
		cg_semicolon();
	}
}

static void
block(void)
{

	if (depth++ > 1)
		error("nesting depth exceeded");

	if (type == TOK_CONST) {
		expect(TOK_CONST);
		if (type == TOK_IDENT) {
			addsymbol(TOK_CONST);
			cg_const();
		}
		expect(TOK_IDENT);
		expect(TOK_EQUAL);
		if (type == TOK_NUMBER) {
			cg_symbol();
			cg_semicolon();
		}
		expect(TOK_NUMBER);
		while (type == TOK_COMMA) {
			expect(TOK_COMMA);
			if (type == TOK_IDENT) {
				addsymbol(TOK_CONST);
				cg_const();
			}
			expect(TOK_IDENT);
			expect(TOK_EQUAL);
			if (type == TOK_NUMBER) {
				cg_symbol();
				cg_semicolon();
			}
			expect(TOK_NUMBER);
		}
		expect(TOK_SEMICOLON);
	}

	if (type == TOK_VAR) {
		expect(TOK_VAR);
		if (type == TOK_IDENT) {
			addsymbol(TOK_VAR);
			cg_var();
		}
		expect(TOK_IDENT);
		if (type == TOK_SIZE) {
			expect(TOK_SIZE);
			if (type == TOK_NUMBER) {
				arraysize();
				cg_array();
				expect(TOK_NUMBER);
			}
		}
		cg_semicolon();
		while (type == TOK_COMMA) {
			expect(TOK_COMMA);
			if (type == TOK_IDENT) {
				addsymbol(TOK_VAR);
				cg_var();
			}
			expect(TOK_IDENT);
			if (type == TOK_SIZE) {
				expect(TOK_SIZE);
				if (type == TOK_NUMBER) {
					arraysize();
					cg_array();
					expect(TOK_NUMBER);
				}
			}
			cg_semicolon();
		}
		expect(TOK_SEMICOLON);
		cg_crlf();
	}

	while (type == TOK_FORWARD) {
		expect(TOK_FORWARD);
		if (type == TOK_IDENT) {
			addsymbol(TOK_FORWARD);
			cg_forward();
		}
		expect(TOK_IDENT);
		expect(TOK_SEMICOLON);
	}

	while (type == TOK_PROCEDURE) {
		proc = 1;

		expect(TOK_PROCEDURE);
		if (type == TOK_IDENT) {
			addsymbol(TOK_PROCEDURE);
			cg_procedure();
		}
		expect(TOK_IDENT);
		expect(TOK_SEMICOLON);

		block();

		expect(TOK_SEMICOLON);

		proc = 0;

		destroysymbols();
	}

	if (proc == 0)
		cg_procedure();

	statement();

	cg_epilogue();

	if (--depth < 0)
		error("nesting depth fell below 0");
}

static void
parse(void)
{

	cg_init();

	next();
	block();
	expect(TOK_DOT);

	if (type != 0)
		error("extra tokens at end of file");

	cg_end();
}

/*
 * Main.
 */

int
main(int argc, char *argv[])
{

	if (argc != 2) {
		(void) fputs("usage: pl0c file.pl0\n", stderr);
		exit(1);
	}

	readin(argv[1]);

	initsymtab();

	parse();

	return 0;
}
