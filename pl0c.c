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

#define PL0C_VERSION	"1.0.0"

#define TOK_IDENT	256
#define TOK_NUMBER	257
#define TOK_CONST	258
#define TOK_VAR		259
#define TOK_PROCEDURE	260
#define TOK_CALL	261
#define TOK_BEGIN	262
#define TOK_END		263
#define TOK_IF		264
#define TOK_THEN	265
#define TOK_WHILE	266
#define TOK_DO		267
#define TOK_ODD		268
#define TOK_DOT		269
#define TOK_EQUAL	270
#define TOK_COMMA	271
#define TOK_SEMICOLON	272
#define TOK_ASSIGN	273
#define TOK_HASH	274
#define TOK_LESSTHAN	275
#define TOK_GREATERTHAN	276
#define TOK_PLUS	277
#define TOK_MINUS	278
#define TOK_MULTIPLY	279
#define TOK_DIVIDE	280
#define TOK_LPAREN	281
#define TOK_RPAREN	282

/*
 * pl0c -- PL/0 compiler.
 *
 * program	= block "." .
 * block	= [ "const" ident "=" number { "," ident "=" number } ";" ]
 *		  [ "var" ident { "," ident } ";" ]
 *		  { "procedure" ident ";" block ";" } statement .
 * statement	= [ ident ":=" expression
 *		  | "call" ident
 *		  | "begin" statement { ";" statement } "end"
 *		  | "if" condition "then" statement
 *		  | "while" condition "do" statement ] .
 * condition	= "odd" expression
 *		| expression ( "=" | "#" | "<" | ">" ) expression .
 * expression	= [ "+" | "-" ] term { ( "+" | "-" ) term } .
 * term		= factor { ( "*" | "/" ) factor } .
 * factor	= ident
 *		| number
 *		| "(" expression ")" .
 */

static char *pname, *raw, *token;
static int depth, first, proc, tok;
static size_t len, line = 1;

struct symtab {
	int depth;
	int type;
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
	size_t i;

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
	else if (!strcmp(token, "while"))
		return TOK_WHILE;
	else if (!strcmp(token, "do"))
		return TOK_DO;
	else if (!strcmp(token, "odd"))
		return TOK_ODD;

	return TOK_IDENT;
}

static int
number(void)
{
	const char *errstr;
	char *p;
	size_t i;

	p = raw;
	while (isdigit(*raw))
		++raw;

	len = raw - p;

	if (isalpha(*raw) || *raw == '_')
		error("unknown character at end of number: '%c'", *raw);

	--raw;

	free(token);

	if ((token = malloc(len + 1)) == NULL)
		error("malloc failed");

	for (i = 0; i < len; i++)
		token[i] = *p++;
	token[i] = '\0';

	(void) strtonum(token, 0, LONG_MAX, &errstr);
	if (errstr != NULL)
		error("invalid number: %s", token);

	return TOK_NUMBER;
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
		return TOK_DOT;
	case '=':
		return TOK_EQUAL;
	case ',':
		return TOK_COMMA;
	case ';':
		return TOK_SEMICOLON;
	case ':':
		if (*++raw != '=')
			error("unknown token: ':%c'", *raw);

		return TOK_ASSIGN;
	case '#':
		return TOK_HASH;
	case '<':
		return TOK_LESSTHAN;
	case '>':
		return TOK_GREATERTHAN;
	case '+':
		return TOK_PLUS;
	case '-':
		return TOK_MINUS;
	case '*':
		return TOK_MULTIPLY;
	case '/':
		return TOK_DIVIDE;
	case '(':
		return TOK_LPAREN;
	case ')':
		return TOK_RPAREN;
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
cg_call(void)
{

	aout("%s();\n", token);
}

static void
cg_const(void)
{

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

	if (proc == 0)
		aout("return 0;");

	aout("}\n\n");
}

static void
cg_odd(void)
{

	aout(")&1");
}

static void
cg_prologue(void)
{

	if (proc == 0) {
		aout("int\n");
		aout("main(int argc, char *argv[])\n");
	} else {
		aout("void\n");
		aout("%s(void)\n", pname);
	}

	aout("{\n");
}

static void
cg_semicolon(void)
{

	aout(";\n");
}

static void
cg_symbol(void)
{

	switch (tok) {
	case TOK_IDENT:
	case TOK_NUMBER:
	case TOK_CONST:
		aout("%s", token);
		break;
	case TOK_BEGIN:
		aout("{\n");
		break;
	case TOK_END:
		aout(";}\n");
		break;
	case TOK_IF:
		aout("if(");
		break;
	case TOK_THEN:
	case TOK_DO:
		aout(")");
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
	case TOK_COMMA:
		aout(",");
		break;
	case TOK_ASSIGN:
		aout("=");
		break;
	case TOK_HASH:
		aout("!=");
		break;
	case TOK_LESSTHAN:
		aout("<");
		break;
	case TOK_GREATERTHAN:
		aout(">");
		break;
	case TOK_PLUS:
		aout("+");
		break;
	case TOK_MINUS:
		aout("-");
		break;
	case TOK_MULTIPLY:
		aout("*");
		break;
	case TOK_DIVIDE:
		aout("/");
		break;
	case TOK_LPAREN:
		aout("(");
		break;
	case TOK_RPAREN:
		aout(")");
	}
}

static void
cg_var_first(void)
{

	aout("%s", token);
}

static void
cg_var_more(void)
{

	aout(", %s", token);
}

static void
cg_var_start(void)
{

	aout("long ");
}

/*
 * Semantics.
 */

static void
symcheck(void)
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
}

/*
 * Parser.
 */

static void
next(void)
{

	tok = lex();
	++raw;
}

static void
expect(int match)
{

	if (match != tok)
		error("syntax error");

	next();
}

static void
addsymbol(int type)
{
	struct symtab *curr, *new;

	curr = head;
	while (curr->next != NULL) {
		if (!strcmp(curr->name, token)) {
			if (curr->depth == (depth - 1))
				error("duplicate symbol: %s", token);
		}
		curr = curr->next;
	}

	if ((new = malloc(sizeof(struct symtab))) == NULL)
		error("malloc failed");

	new->depth = depth - 1;
	new->type = type;
	new->name = strdup(token);
	new->next = NULL;

	curr->next = new;

	expect(TOK_IDENT);
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
	new->name = "_start";
	new->next = NULL;

	head = new;
}

static void
checkconst(void)
{
	struct symtab *curr;
	const char *errstr;

	curr = head;
	while (curr->next != NULL)
		curr = curr->next;

	(void) strtonum(token, LONG_MIN, LONG_MAX, &errstr);
	if (errstr != NULL)
		error("unknown number: %s", token);

	expect(TOK_NUMBER);
}

static void
factor(void)
{

	switch (tok) {
	case TOK_IDENT:
		symcheck();
		/* Fallthru */
	case TOK_NUMBER:
		cg_symbol();
		next();
		break;
	case TOK_LPAREN:
		cg_symbol();
		expect(TOK_LPAREN);
		expression();
		if (tok == TOK_RPAREN)
			cg_symbol();
		expect(TOK_RPAREN);
	}
}

static void
term(void)
{

	factor();

	while (tok == TOK_MULTIPLY || tok == TOK_DIVIDE) {
		cg_symbol();
		next();
		factor();
	}
}

static void
expression(void)
{

	if (tok == TOK_PLUS || tok == TOK_MINUS) {
		cg_symbol();
		next();
	}

	term();

	while (tok == TOK_PLUS || tok == TOK_MINUS) {
		cg_symbol();
		next();
		term();
	}
}

static void
condition(void)
{

	if (tok == TOK_ODD) {
		cg_symbol();
		expect(TOK_ODD);
		expression();
		cg_odd();
	} else {
		expression();

		switch (tok) {
		case TOK_EQUAL:
		case TOK_HASH:
		case TOK_LESSTHAN:
		case TOK_GREATERTHAN:
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
	struct symtab *left;

	switch (tok) {
	case TOK_IDENT:
		symcheck();
		cg_symbol();
		expect(TOK_IDENT);
		if (tok == TOK_ASSIGN)
			cg_symbol();
		expect(TOK_ASSIGN);
		expression();
		break;
	case TOK_CALL:
		expect(TOK_CALL);
		if (tok == TOK_IDENT)
			cg_call();
		expect(TOK_IDENT);
		break;
	case TOK_BEGIN:
		cg_symbol();
		expect(TOK_BEGIN);
		statement();
		while (tok == TOK_SEMICOLON) {
			cg_semicolon();
			expect(TOK_SEMICOLON);
			statement();
		}
		if (tok == TOK_END)
			cg_symbol();
		expect(TOK_END);
		break;
	case TOK_IF:
		cg_symbol();
		expect(TOK_IF);
		condition();
		if (tok == TOK_THEN)
			cg_symbol();
		expect(TOK_THEN);
		statement();
		break;
	case TOK_WHILE:
		cg_symbol();
		expect(TOK_WHILE);
		condition();
		if (tok == TOK_DO)
			cg_symbol();
		expect(TOK_DO);
		statement();
	}
}

static void
block(void)
{
	int locals = 0;
	size_t no = 0;

	if (depth++ > 1)
		error("nesting depth exceeded");

	if (tok == TOK_CONST) {
		expect(TOK_CONST);
		if (tok == TOK_IDENT)
			cg_const();
		addsymbol(TOK_CONST);
		expect(TOK_EQUAL);
		if (tok == TOK_NUMBER) {
			cg_symbol();
			cg_semicolon();
		}
		checkconst();
		while (tok == TOK_COMMA) {
			expect(TOK_COMMA);
			if (tok == TOK_IDENT)
				cg_const();
			addsymbol(TOK_CONST);
			expect(TOK_EQUAL);
			if (tok == TOK_NUMBER) {
				cg_symbol();
				cg_semicolon();
			}
			checkconst();
		}
		expect(TOK_SEMICOLON);
	}

	if (tok == TOK_VAR) {
		cg_var_start();
		expect(TOK_VAR);
		if (tok == TOK_IDENT)
			cg_var_first();
		addsymbol(TOK_VAR);
		while (tok == TOK_COMMA) {
			expect(TOK_COMMA);
			if (tok == TOK_IDENT)
				cg_var_more();
			addsymbol(TOK_VAR);
		}
		expect(TOK_SEMICOLON);
		cg_semicolon();
		cg_crlf();
	}

	while (tok == TOK_PROCEDURE) {
		proc = 1;

		expect(TOK_PROCEDURE);
		if (tok == TOK_IDENT)
			pname = strdup(token);
		addsymbol(TOK_PROCEDURE);
		cg_prologue();
		expect(TOK_SEMICOLON);

		block();

		expect(TOK_SEMICOLON);

		free(pname);
		pname = NULL;
		proc = 0;

		destroysymbols();
	}

	if (proc == 0)
		cg_prologue();

	statement();

	cg_epilogue();

	if (--depth < 0)
		error("nesting depth fell below 0");
}

static void
parse(void)
{

	next();
	block();
	expect(TOK_DOT);

	if (tok != 0)
		error("extra tokens at end of file");

	cg_end();
}

/*
 * Main.
 */

int
main(int argc, char *argv[])
{
	struct symtab *curr;
	char *startp;

	if (argc != 2) {
		(void) fputs("usage: pl0c file.pl0\n", stderr);
		exit(1);
	}

	readin(argv[1]);
	startp = raw;

	initsymtab();

	parse();

	free(startp);

	return 0;
}
