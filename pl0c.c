#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char __stdin[24];
static const char *__errstr;

static long __writestridx;

#ifndef HAVE_STRTONUM
#include "strtonum.c"
#endif

static const long CHECK_LHS = 0;
static const long CHECK_RHS = 1;
static const long CHECK_CALL = 2;
static const long TOK_IDENT = 'I';
static const long TOK_NUMBER = 'N';
static const long TOK_CONST = 'C';
static const long TOK_VAR = 'V';
static const long TOK_PROCEDURE = 'P';
static const long TOK_CALL = 'c';
static const long TOK_BEGIN = 'B';
static const long TOK_END = 'E';
static const long TOK_IF = 'i';
static const long TOK_THEN = 'T';
static const long TOK_ELSE = 'e';
static const long TOK_WHILE = 'W';
static const long TOK_DO = 'D';
static const long TOK_ODD = 'O';
static const long TOK_WRITEINT = 'w';
static const long TOK_WRITECHAR = 'H';
static const long TOK_WRITESTR = 'S';
static const long TOK_READINT = 'R';
static const long TOK_READCHAR = 'h';
static const long TOK_INTO = 'n';
static const long TOK_SIZE = 's';
static const long TOK_EXIT = 'X';
static const long TOK_AND = '&';
static const long TOK_OR = '|';
static const long TOK_NOT = '~';
static const long TOK_FORWARD = 'F';
static const long TOK_DOT = '.';
static const long TOK_EQUAL = '=';
static const long TOK_COMMA = ',';
static const long TOK_SEMICOLON = ';';
static const long TOK_ASSIGN = ':';
static const long TOK_HASH = '#';
static const long TOK_LTHAN = '<';
static const long TOK_GTHAN = '>';
static const long TOK_LTHANE = '{';
static const long TOK_GTHANE = '}';
static const long TOK_PLUS = '+';
static const long TOK_MINUS = '-';
static const long TOK_MULTIPLY = '*';
static const long TOK_DIVIDE = '/';
static const long TOK_MODULO = '%';
static const long TOK_LPAREN = '(';
static const long TOK_RPAREN = ')';
static const long TOK_LBRACK = '[';
static const long TOK_RBRACK = ']';
static const long TOK_STRING = '"';
static long raw[1048576];
static long loc;
static long symtab[1048576];
static long symtype;
static long token[256];
static long type;
static long str[256];
static long symtabcnt;
static long ret;
static long keywords[256];
static long keywordidx[64];
static long depth;
static long proc;
static long value;
static long line;

static void expression(void);
static void error(void) {
  {
    (void)fprintf(stdout, "pl0c: error: ");
    ;
    (void)fprintf(stdout, "%ld", (long)(line));
    ;
    (void)fprintf(stdout, ": ");
    ;
    ;
  };
}

static void setupkeywords(void) {
  long i;

  {
    i = 0;
    keywordidx[0] = TOK_CONST;
    keywordidx[1] = i;
    keywords[i + 0] = 'c';
    keywords[i + 1] = 'o';
    keywords[i + 2] = 'n';
    keywords[i + 3] = 's';
    keywords[i + 4] = 't';
    keywords[i + 5] = '\0';
    i = i + 6;
    keywordidx[2] = TOK_VAR;
    keywordidx[3] = i;
    keywords[i + 0] = 'v';
    keywords[i + 1] = 'a';
    keywords[i + 2] = 'r';
    keywords[i + 3] = '\0';
    i = i + 4;
    keywordidx[4] = TOK_PROCEDURE;
    keywordidx[5] = i;
    keywords[i + 0] = 'p';
    keywords[i + 1] = 'r';
    keywords[i + 2] = 'o';
    keywords[i + 3] = 'c';
    keywords[i + 4] = 'e';
    keywords[i + 5] = 'd';
    keywords[i + 6] = 'u';
    keywords[i + 7] = 'r';
    keywords[i + 8] = 'e';
    keywords[i + 9] = '\0';
    i = i + 10;
    keywordidx[6] = TOK_CALL;
    keywordidx[7] = i;
    keywords[i + 0] = 'c';
    keywords[i + 1] = 'a';
    keywords[i + 2] = 'l';
    keywords[i + 3] = 'l';
    keywords[i + 4] = '\0';
    i = i + 5;
    keywordidx[8] = TOK_BEGIN;
    keywordidx[9] = i;
    keywords[i + 0] = 'b';
    keywords[i + 1] = 'e';
    keywords[i + 2] = 'g';
    keywords[i + 3] = 'i';
    keywords[i + 4] = 'n';
    keywords[i + 5] = '\0';
    i = i + 6;
    keywordidx[10] = TOK_END;
    keywordidx[11] = i;
    keywords[i + 0] = 'e';
    keywords[i + 1] = 'n';
    keywords[i + 2] = 'd';
    keywords[i + 3] = '\0';
    i = i + 4;
    keywordidx[12] = TOK_IF;
    keywordidx[13] = i;
    keywords[i + 0] = 'i';
    keywords[i + 1] = 'f';
    keywords[i + 2] = '\0';
    i = i + 3;
    keywordidx[14] = TOK_THEN;
    keywordidx[15] = i;
    keywords[i + 0] = 't';
    keywords[i + 1] = 'h';
    keywords[i + 2] = 'e';
    keywords[i + 3] = 'n';
    keywords[i + 4] = '\0';
    i = i + 5;
    keywordidx[16] = TOK_ELSE;
    keywordidx[17] = i;
    keywords[i + 0] = 'e';
    keywords[i + 1] = 'l';
    keywords[i + 2] = 's';
    keywords[i + 3] = 'e';
    keywords[i + 4] = '\0';
    i = i + 5;
    keywordidx[18] = TOK_WHILE;
    keywordidx[19] = i;
    keywords[i + 0] = 'w';
    keywords[i + 1] = 'h';
    keywords[i + 2] = 'i';
    keywords[i + 3] = 'l';
    keywords[i + 4] = 'e';
    keywords[i + 5] = '\0';
    i = i + 6;
    keywordidx[20] = TOK_DO;
    keywordidx[21] = i;
    keywords[i + 0] = 'd';
    keywords[i + 1] = 'o';
    keywords[i + 2] = '\0';
    i = i + 3;
    keywordidx[22] = TOK_ODD;
    keywordidx[23] = i;
    keywords[i + 0] = 'o';
    keywords[i + 1] = 'd';
    keywords[i + 2] = 'd';
    keywords[i + 3] = '\0';
    i = i + 4;
    keywordidx[24] = TOK_WRITEINT;
    keywordidx[25] = i;
    keywords[i + 0] = 'w';
    keywords[i + 1] = 'r';
    keywords[i + 2] = 'i';
    keywords[i + 3] = 't';
    keywords[i + 4] = 'e';
    keywords[i + 5] = 'I';
    keywords[i + 6] = 'n';
    keywords[i + 7] = 't';
    keywords[i + 8] = '\0';
    i = i + 9;
    keywordidx[26] = TOK_WRITECHAR;
    keywordidx[27] = i;
    keywords[i + 0] = 'w';
    keywords[i + 1] = 'r';
    keywords[i + 2] = 'i';
    keywords[i + 3] = 't';
    keywords[i + 4] = 'e';
    keywords[i + 5] = 'C';
    keywords[i + 6] = 'h';
    keywords[i + 7] = 'a';
    keywords[i + 8] = 'r';
    keywords[i + 9] = '\0';
    i = i + 10;
    keywordidx[28] = TOK_WRITESTR;
    keywordidx[29] = i;
    keywords[i + 0] = 'w';
    keywords[i + 1] = 'r';
    keywords[i + 2] = 'i';
    keywords[i + 3] = 't';
    keywords[i + 4] = 'e';
    keywords[i + 5] = 'S';
    keywords[i + 6] = 't';
    keywords[i + 7] = 'r';
    keywords[i + 8] = '\0';
    i = i + 9;
    keywordidx[30] = TOK_READINT;
    keywordidx[31] = i;
    keywords[i + 0] = 'r';
    keywords[i + 1] = 'e';
    keywords[i + 2] = 'a';
    keywords[i + 3] = 'd';
    keywords[i + 4] = 'I';
    keywords[i + 5] = 'n';
    keywords[i + 6] = 't';
    keywords[i + 7] = '\0';
    i = i + 8;
    keywordidx[32] = TOK_READCHAR;
    keywordidx[33] = i;
    keywords[i + 0] = 'r';
    keywords[i + 1] = 'e';
    keywords[i + 2] = 'a';
    keywords[i + 3] = 'd';
    keywords[i + 4] = 'C';
    keywords[i + 5] = 'h';
    keywords[i + 6] = 'a';
    keywords[i + 7] = 'r';
    keywords[i + 8] = '\0';
    i = i + 9;
    keywordidx[34] = TOK_INTO;
    keywordidx[35] = i;
    keywords[i + 0] = 'i';
    keywords[i + 1] = 'n';
    keywords[i + 2] = 't';
    keywords[i + 3] = 'o';
    keywords[i + 4] = '\0';
    i = i + 5;
    keywordidx[36] = TOK_SIZE;
    keywordidx[37] = i;
    keywords[i + 0] = 's';
    keywords[i + 1] = 'i';
    keywords[i + 2] = 'z';
    keywords[i + 3] = 'e';
    keywords[i + 4] = '\0';
    i = i + 5;
    keywordidx[38] = TOK_EXIT;
    keywordidx[39] = i;
    keywords[i + 0] = 'e';
    keywords[i + 1] = 'x';
    keywords[i + 2] = 'i';
    keywords[i + 3] = 't';
    keywords[i + 4] = '\0';
    i = i + 5;
    keywordidx[40] = TOK_AND;
    keywordidx[41] = i;
    keywords[i + 0] = 'a';
    keywords[i + 1] = 'n';
    keywords[i + 2] = 'd';
    keywords[i + 3] = '\0';
    i = i + 4;
    keywordidx[42] = TOK_OR;
    keywordidx[43] = i;
    keywords[i + 0] = 'o';
    keywords[i + 1] = 'r';
    keywords[i + 2] = '\0';
    i = i + 3;
    keywordidx[44] = TOK_NOT;
    keywordidx[45] = i;
    keywords[i + 0] = 'n';
    keywords[i + 1] = 'o';
    keywords[i + 2] = 't';
    keywords[i + 3] = '\0';
    i = i + 4;
    keywordidx[46] = TOK_MODULO;
    keywordidx[47] = i;
    keywords[i + 0] = 'm';
    keywords[i + 1] = 'o';
    keywords[i + 2] = 'd';
    keywords[i + 3] = '\0';
    i = i + 4;
    keywordidx[48] = TOK_FORWARD;
    keywordidx[49] = i;
    keywords[i + 0] = 'f';
    keywords[i + 1] = 'o';
    keywords[i + 2] = 'r';
    keywords[i + 3] = 'w';
    keywords[i + 4] = 'a';
    keywords[i + 5] = 'r';
    keywords[i + 6] = 'd';
    keywords[i + 7] = '\0';
    i = i + 8;
    keywordidx[50] = -1;
    ;
  };
}

static void findkeyword(void) {
  long keyword;
  long k;
  long i;
  long eq;

  {
    keyword = 0;
    ret = 1;
    while ((ret * (keywordidx[keyword * 2] + 1)) != 0) {
      type = keywordidx[keyword * 2];
      k = keywordidx[keyword * 2 + 1];
      eq = 1;
      i = 0;
      while ((eq * keywords[k]) != 0) {
        if (keywords[k] != token[i]) {
          eq = 0;
        };
        k = k + 1;
        i = i + 1;
      };
      if (token[i] != '\0') {
        eq = 0;
      };
      if (eq == 1) {
        ret = 0;
      };
      keyword = keyword + 1;
      ;
    };
    ;
  };
}

static void cmpstr(void) {
  long i;

  {
    i = 0;
    ret = 0;
    while (i < 256) {
      if (token[i] != str[i]) {
        ret = 1;
      };
      i = i + 1;
    };
    ;
  };
}

static void clrstr(void) {
  long i;

  {
    i = 0;
    while (i < 256) {
      str[i] = '\0';
      i = i + 1;
    };
    ;
  };
}

static void readin(void) {
  long ch;
  long i;

  {
    i = 0;
    ch = (int)fgetc(stdin);
    ;
    while (ch != -1) {
      raw[i] = ch;
      i = i + 1;
      if (i == 1048577) {
        {
          error();
          ;
          (void)fprintf(stdout, "file too big\n");
          ;
          exit(1);
          ;
          ;
        };
      };
      ch = (int)fgetc(stdin);
      ;
      ;
    };
    loc = 0;
  };
}

static void comment(void) {
  {
    while (raw[loc] != '}') {
      if (raw[loc] == '\0') {
        {
          error();
          ;
          (void)fprintf(stdout, "unterminated comment\n");
          ;
          exit(1);
          ;
          ;
        };
      } else {
        if (raw[loc] == '\n') {
          line = line + 1;
        };
      };
      loc = loc + 1;
    };
    ;
  };
}

static void ident(void) {
  long i;
  long loop;

  {
    i = 0;
    loop = 1;
    while (i < 256) {
      token[i] = '\0';
      i = i + 1;
    };
    i = 0;
    while (loop != 0) {
      loop = 0;
      if (raw[loc] >= 'A') {
        {
          if (raw[loc] <= 'Z') {
            {
              loop = 1;
              token[i] = raw[loc];
              i = i + 1;
              ;
            };
          };
          ;
        };
      };
      if (loop == 0) {
        {
          if (raw[loc] >= 'a') {
            {
              if (raw[loc] <= 'z') {
                {
                  loop = 1;
                  token[i] = raw[loc];
                  i = i + 1;
                  ;
                };
              };
              ;
            };
          };
          ;
        };
      };
      if (loop == 0) {
        {
          if (raw[loc] >= '0') {
            {
              if (raw[loc] <= '9') {
                {
                  loop = 1;
                  token[i] = raw[loc];
                  i = i + 1;
                  ;
                };
              };
              ;
            };
          };
          ;
        };
      };
      if (loop == 0) {
        {
          if (raw[loc] == '_') {
            {
              loop = 1;
              token[i] = raw[loc];
              i = i + 1;
              ;
            };
          };
          ;
        };
      };
      if (i >= 256) {
        {
          error();
          ;
          (void)fprintf(stdout, "token too long\n");
          ;
          exit(1);
          ;
          ;
        };
      };
      if (loop != 0) {
        loc = loc + 1;
      };
      ;
    };
    loc = loc - 1;
    findkeyword();
    ;
    if (ret != 0) {
      type = TOK_IDENT;
    };
    ;
  };
}

static void number(void) {
  long i;
  long loop;

  {
    i = 0;
    loop = 1;
    value = 0;
    while (i < 256) {
      token[i] = '\0';
      i = i + 1;
    };
    i = 0;
    while (loop != 0) {
      loop = 0;
      if (raw[loc] >= '0') {
        {
          if (raw[loc] <= '9') {
            {
              loop = 1;
              token[i] = raw[loc];
              i = i + 1;
              if (i == 256) {
                {
                  error();
                  ;
                  (void)fprintf(stdout, "token too long\n");
                  ;
                  exit(1);
                  ;
                  ;
                };
              };
              value = value * 10;
              value = value + (raw[loc] - '0');
            };
          };
          ;
        };
      };
      if (loop == 0) {
        {
          if (raw[loc] == '_') {
            loop = 1;
          };
          ;
        };
      };
      if (loop != 0) {
        loc = loc + 1;
      };
      ;
    };
    loc = loc - 1;
    type = TOK_NUMBER;
    ;
  };
}

static void string(void) {
  long i;
  long loop;

  {
    i = 0;
    loop = 1;
    while (i < 256) {
      token[i] = '\0';
      i = i + 1;
    };
    i = 1;
    type = TOK_STRING;
    loc = loc + 1;
    token[0] = '"';
    while (loop != 0) {
      if (raw[loc] == '\'') {
        {
          loc = loc + 1;
          if (raw[loc] != '\'') {
            loop = 0;
          } else {
            {
              token[i] = '\\';
              i = i + 1;
              token[i] = raw[loc];
              i = i + 1;
            };
          };
          ;
        };
      } else {
        if (raw[loc] == '\n') {
          {
            error();
            ;
            (void)fprintf(stdout, "unterminated string\n");
            ;
            exit(1);
            ;
            ;
          };
        } else {
          if (raw[loc] == '\0') {
            {
              error();
              ;
              (void)fprintf(stdout, "unterminated string\n");
              ;
              exit(1);
              ;
              ;
            };
          } else {
            {
              token[i] = raw[loc];
              i = i + 1;
            };
          };
        };
      };
      if (i > 256) {
        {
          error();
          ;
          (void)fprintf(stdout, "string too long\n");
          ;
          exit(1);
          ;
          ;
        };
      };
      if (loop != 0) {
        loc = loc + 1;
      };
      ;
    };
    loc = loc - 1;
    token[i] = '"';
    if (i == 2) {
      {
        token[0] = '\'';
        token[2] = '\'';
        type = TOK_NUMBER;
      };
    } else {
      if (i == 3) {
        {
          if (token[1] == '\\') {
            {
              token[0] = '\'';
              token[3] = '\'';
              type = TOK_NUMBER;
            };
          };
          ;
        };
      };
    };
    ;
  };
}

static void lex(void) {
  long i;
  long whitespace;
  long isident;
  long isnumber;

  {
    isident = 0;
    isnumber = 0;
    i = 0;
    whitespace = 1;
    while (whitespace == 1) {
      if (raw[loc] == ' ') {
        {
          whitespace = 1;
          loc = loc + 1;
          ;
        };
      } else {
        if (raw[loc] == '\t') {
          {
            whitespace = 1;
            loc = loc + 1;
            ;
          };
        } else {
          if (raw[loc] == '\n') {
            {
              whitespace = 1;
              loc = loc + 1;
              line = line + 1;
              ;
            };
          } else {
            whitespace = 0;
          };
        };
      };
    };
    if (raw[loc] >= 'A') {
      {
        if (raw[loc] <= 'Z') {
          {
            isident = 1;
            ident();
            ;
            ;
          };
        };
        ;
      };
    };
    if (isident == 0) {
      {
        if (raw[loc] >= 'a') {
          {
            if (raw[loc] <= 'z') {
              {
                isident = 1;
                ident();
                ;
                ;
              };
            };
            ;
          };
        };
        ;
      };
    };
    if (isident == 0) {
      {
        if (raw[loc] == '_') {
          {
            isident = 1;
            ident();
            ;
            ;
          };
        };
        ;
      };
    };
    if (isident == 0) {
      {
        if (raw[loc] >= '0') {
          {
            if (raw[loc] <= '9') {
              {
                isnumber = 1;
                number();
                ;
                ;
              };
            };
            ;
          };
        };
        ;
      };
    };
    if (isident == 0) {
      {
        if (isnumber == 0) {
          {
            if (raw[loc] == '{') {
              {
                comment();
                ;
                type = -1;
              };
            } else {
              if (raw[loc] == '.') {
                type = TOK_DOT;
              } else {
                if (raw[loc] == '=') {
                  type = TOK_EQUAL;
                } else {
                  if (raw[loc] == ',') {
                    type = TOK_COMMA;
                  } else {
                    if (raw[loc] == ';') {
                      type = TOK_SEMICOLON;
                    } else {
                      if (raw[loc] == '#') {
                        type = TOK_HASH;
                      } else {
                        if (raw[loc] == '+') {
                          type = TOK_PLUS;
                        } else {
                          if (raw[loc] == '-') {
                            type = TOK_MINUS;
                          } else {
                            if (raw[loc] == '*') {
                              type = TOK_MULTIPLY;
                            } else {
                              if (raw[loc] == '/') {
                                type = TOK_DIVIDE;
                              } else {
                                if (raw[loc] == '%') {
                                  type = TOK_MODULO;
                                } else {
                                  if (raw[loc] == '(') {
                                    type = TOK_LPAREN;
                                  } else {
                                    if (raw[loc] == ')') {
                                      type = TOK_RPAREN;
                                    } else {
                                      if (raw[loc] == '[') {
                                        type = TOK_LBRACK;
                                      } else {
                                        if (raw[loc] == ']') {
                                          type = TOK_RBRACK;
                                        } else {
                                          if (raw[loc] == '<') {
                                            {
                                              loc = loc + 1;
                                              if (raw[loc] == '=') {
                                                type = TOK_LTHANE;
                                              } else {
                                                if (raw[loc] == '>') {
                                                  type = TOK_HASH;
                                                } else {
                                                  {
                                                    loc = loc - 1;
                                                    type = TOK_LTHAN;
                                                  };
                                                };
                                              };
                                              ;
                                            };
                                          } else {
                                            if (raw[loc] == '>') {
                                              {
                                                loc = loc + 1;
                                                if (raw[loc] == '=') {
                                                  type = TOK_GTHANE;
                                                } else {
                                                  {
                                                    loc = loc - 1;
                                                    type = TOK_GTHAN;
                                                  };
                                                };
                                                ;
                                              };
                                            } else {
                                              if (raw[loc] == ':') {
                                                {
                                                  loc = loc + 1;
                                                  if (raw[loc] != '=') {
                                                    {
                                                      error();
                                                      ;
                                                      (void)fprintf(
                                                          stdout,
                                                          "unknown token: \'");
                                                      ;
                                                      (void)fprintf(
                                                          stdout, "%c",
                                                          (unsigned char)(raw[loc]));
                                                      ;
                                                      (void)fprintf(stdout,
                                                                    "\'\n");
                                                      ;
                                                      exit(1);
                                                      ;
                                                      ;
                                                    };
                                                  };
                                                  type = TOK_ASSIGN;
                                                  ;
                                                };
                                              } else {
                                                if (raw[loc] == '\'') {
                                                  {
                                                    string();
                                                    ;
                                                    ;
                                                  };
                                                } else {
                                                  if (raw[loc] == '\0') {
                                                    type = 0;
                                                  } else {
                                                    {
                                                      error();
                                                      ;
                                                      (void)fprintf(
                                                          stdout,
                                                          "unknown token: \'");
                                                      ;
                                                      (void)fprintf(
                                                          stdout, "%c",
                                                          (unsigned char)(raw[loc]));
                                                      ;
                                                      (void)fprintf(stdout,
                                                                    "\'\n");
                                                      ;
                                                      exit(1);
                                                      ;
                                                      ;
                                                    };
                                                  };
                                                };
                                              };
                                            };
                                          };
                                        };
                                      };
                                    };
                                  };
                                };
                              };
                            };
                          };
                        };
                      };
                    };
                  };
                };
              };
            };
            ;
          };
        };
        ;
      };
    };
    loc = loc + 1;
  };
}

static void cg_array(void) {
  {
    (void)fprintf(stdout, "%c", (unsigned char)('['));
    ;
    __writestridx = 0;
    while (token[__writestridx] != '\0' && __writestridx < 256)
      (void)fputc((unsigned char)token[__writestridx++], stdout);
    ;
    (void)fprintf(stdout, "%c", (unsigned char)(']'));
    ;
  };
}

static void cg_call(void) {
  {
    __writestridx = 0;
    while (token[__writestridx] != '\0' && __writestridx < 256)
      (void)fputc((unsigned char)token[__writestridx++], stdout);
    ;
    (void)fprintf(stdout, "();\n");
    ;
  };
}

static void cg_closeblock(void) {
  {
    (void)fprintf(stdout, ";}\n");
    ;
  };
}

static void cg_const(void) {
  {
    if (proc == 0) {
      (void)fprintf(stdout, "static ");
      ;
    };
    (void)fprintf(stdout, "const long ");
    ;
    __writestridx = 0;
    while (token[__writestridx] != '\0' && __writestridx < 256)
      (void)fputc((unsigned char)token[__writestridx++], stdout);
    ;
    (void)fprintf(stdout, "%c", (unsigned char)('='));
    ;
  };
}

static void cg_crlf(void) {
  {
    (void)fprintf(stdout, "%c", (unsigned char)('\n'));
    ;
  };
}

static void cg_end(void) {
  {
    (void)fprintf(stdout, "/* PL/0 compiler 1.0.0 */\n");
    ;
  };
}

static void cg_epilogue(void) {
  {
    (void)fprintf(stdout, "%c", (unsigned char)(';'));
    ;
    if (proc == 0) {
      (void)fprintf(stdout, "\nreturn 0;");
      ;
    };
    (void)fprintf(stdout, "\n}\n\n");
    ;
  };
}

static void cg_exit(void) {
  {
    (void)fprintf(stdout, "exit(");
    ;
  };
}

static void cg_forward(void) {
  {
    (void)fprintf(stdout, "static void ");
    ;
    __writestridx = 0;
    while (token[__writestridx] != '\0' && __writestridx < 256)
      (void)fputc((unsigned char)token[__writestridx++], stdout);
    ;
    (void)fprintf(stdout, "(void);\n");
    ;
  };
}

static void cg_init(void) {
  {
    (void)fprintf(stdout, "#include <limits.h>\n");
    ;
    (void)fprintf(stdout, "#include <stdio.h>\n");
    ;
    (void)fprintf(stdout, "#include <stdlib.h>\n");
    ;
    (void)fprintf(stdout, "#include <string.h>\n\n");
    ;
    (void)fprintf(stdout, "static char __stdin[24];\n");
    ;
    (void)fprintf(stdout, "static const char *__errstr;\n\n");
    ;
    (void)fprintf(stdout, "static long __writestridx;\n\n");
    ;
    (void)fprintf(stdout, "#ifndef HAVE_STRTONUM\n");
    ;
    (void)fprintf(stdout, "#include ");
    ;
    (void)fprintf(stdout, "%c", (unsigned char)('"'));
    ;
    (void)fprintf(stdout, "strtonum.c");
    ;
    (void)fprintf(stdout, "%c", (unsigned char)('"'));
    ;
    (void)fprintf(stdout, "%c", (unsigned char)('\n'));
    ;
    (void)fprintf(stdout, "#endif\n\n");
    ;
  };
}

static void cg_odd(void) {
  {
    (void)fprintf(stdout, ")&1");
    ;
  };
}

static void cg_procedure(void) {
  {
    if (proc == 0) {
      {
        (void)fprintf(stdout, "int\n");
        ;
        (void)fprintf(stdout, "main(int argc, char *argv[])\n");
        ;
      };
    } else {
      {
        (void)fprintf(stdout, "static void\n");
        ;
        __writestridx = 0;
        while (token[__writestridx] != '\0' && __writestridx < 256)
          (void)fputc((unsigned char)token[__writestridx++], stdout);
        ;
        (void)fprintf(stdout, "(void)\n");
        ;
      };
    };
    (void)fprintf(stdout, "{\n");
    ;
  };
}

static void cg_readchar(void) {
  {
    __writestridx = 0;
    while (token[__writestridx] != '\0' && __writestridx < 256)
      (void)fputc((unsigned char)token[__writestridx++], stdout);
    ;
    (void)fprintf(stdout, "=(int) fgetc(stdin);");
    ;
  };
}

static void cg_readint(void) {
  {
    (void)fprintf(stdout, "(void) fgets(__stdin, sizeof(__stdin), stdin);\n");
    ;
    (void)fprintf(stdout, "if(__stdin[strlen(__stdin) - 1] == \'\\n\')");
    ;
    (void)fprintf(stdout, "__stdin[strlen(__stdin) - 1] = \'\\0\';");
    ;
    __writestridx = 0;
    while (token[__writestridx] != '\0' && __writestridx < 256)
      (void)fputc((unsigned char)token[__writestridx++], stdout);
    ;
    (void)fprintf(
        stdout, "=(long) strtonum(__stdin, LONG_MIN, LONG_MAX, &__errstr);\n");
    ;
    (void)fprintf(stdout, "if(__errstr!=NULL){");
    ;
    (void)fprintf(stdout, "(void) fprintf(stderr, ");
    ;
    (void)fprintf(stdout, "%c", (unsigned char)('"'));
    ;
    (void)fprintf(stdout, "invalid number: %%s");
    ;
    (void)fprintf(stdout, "%c", (unsigned char)('"'));
    ;
    (void)fprintf(stdout, ", __stdin);\n");
    ;
    (void)fprintf(stdout, "exit(1);");
    ;
    (void)fprintf(stdout, "%c", (unsigned char)('}'));
    ;
  };
}

static void cg_rparen(void) {
  {
    (void)fprintf(stdout, "%c", (unsigned char)(')'));
    ;
  };
}

static void cg_semicolon(void) {
  {
    (void)fprintf(stdout, ";\n");
    ;
  };
}

static void cg_symbol(void) {
  {
    if (type == TOK_IDENT) {
      __writestridx = 0;
      while (token[__writestridx] != '\0' && __writestridx < 256)
        (void)fputc((unsigned char)token[__writestridx++], stdout);
      ;
    } else {
      if (type == TOK_NUMBER) {
        __writestridx = 0;
        while (token[__writestridx] != '\0' && __writestridx < 256)
          (void)fputc((unsigned char)token[__writestridx++], stdout);
        ;
      } else {
        if (type == TOK_BEGIN) {
          (void)fprintf(stdout, "{\n");
          ;
        } else {
          if (type == TOK_END) {
            (void)fprintf(stdout, ";\n}\n");
            ;
          } else {
            if (type == TOK_IF) {
              (void)fprintf(stdout, "if(");
              ;
            } else {
              if (type == TOK_THEN) {
                (void)fprintf(stdout, "){");
                ;
              } else {
                if (type == TOK_DO) {
                  (void)fprintf(stdout, "%c", (unsigned char)(')'));
                  ;
                } else {
                  if (type == TOK_ELSE) {
                    (void)fprintf(stdout, ";}else{");
                    ;
                  } else {
                    if (type == TOK_ODD) {
                      (void)fprintf(stdout, "%c", (unsigned char)('('));
                      ;
                    } else {
                      if (type == TOK_WHILE) {
                        (void)fprintf(stdout, "while(");
                        ;
                      } else {
                        if (type == TOK_EQUAL) {
                          (void)fprintf(stdout, "==");
                          ;
                        } else {
                          if (type == TOK_ASSIGN) {
                            (void)fprintf(stdout, "%c", (unsigned char)('='));
                            ;
                          } else {
                            if (type == TOK_HASH) {
                              (void)fprintf(stdout, "!=");
                              ;
                            } else {
                              if (type == TOK_LTHANE) {
                                (void)fprintf(stdout, "<=");
                                ;
                              } else {
                                if (type == TOK_GTHANE) {
                                  (void)fprintf(stdout, ">=");
                                  ;
                                } else {
                                  (void)fprintf(stdout, "%c",
                                                (unsigned char)(type));
                                  ;
                                };
                              };
                            };
                          };
                        };
                      };
                    };
                  };
                };
              };
            };
          };
        };
      };
    };
    ;
  };
}

static void cg_var(void) {
  {
    if (proc == 0) {
      (void)fprintf(stdout, "static ");
      ;
    };
    (void)fprintf(stdout, "long ");
    ;
    __writestridx = 0;
    while (token[__writestridx] != '\0' && __writestridx < 256)
      (void)fputc((unsigned char)token[__writestridx++], stdout);
    ;
  };
}

static void cg_writechar(void) {
  {
    (void)fprintf(stdout, "(void) fprintf(stdout, ");
    ;
    (void)fprintf(stdout, "%c", (unsigned char)('"'));
    ;
    (void)fprintf(stdout, "%%c");
    ;
    (void)fprintf(stdout, "%c", (unsigned char)('"'));
    ;
    (void)fprintf(stdout, ", (unsigned char) (");
    ;
  };
}

static void cg_writeint(void) {
  {
    (void)fprintf(stdout, "(void) fprintf(stdout, ");
    ;
    (void)fprintf(stdout, "%c", (unsigned char)('"'));
    ;
    (void)fprintf(stdout, "%%ld");
    ;
    (void)fprintf(stdout, "%c", (unsigned char)('"'));
    ;
    (void)fprintf(stdout, ", (long) (");
    ;
  };
}

static void cg_writestr(void) {
  long i;
  long n;
  long p;
  long save;

  {
    i = 0;
    save = 0;
    if (type == TOK_IDENT) {
      {
        while (i < symtabcnt) {
          clrstr();
          ;
          n = 0;
          p = 35 * i;
          while (n < 32) {
            str[n] = symtab[p];
            p = p + 1;
            n = n + 1;
          };
          cmpstr();
          ;
          if (ret == 0) {
            save = 35 * i;
          };
          i = i + 1;
        };
        if (save == 0) {
          {
            error();
            ;
            (void)fprintf(stdout, "undefined symbol: \'");
            ;
            __writestridx = 0;
            while (token[__writestridx] != '\0' && __writestridx < 256)
              (void)fputc((unsigned char)token[__writestridx++], stdout);
            ;
            (void)fprintf(stdout, "\'\n");
            ;
            exit(1);
            ;
            ;
          };
        };
        if (symtab[save + 34] == 0) {
          {
            error();
            ;
            (void)fprintf(stdout, "writeStr requires an array\n");
            ;
            exit(1);
            ;
            ;
          };
        };
        (void)fprintf(stdout, "__writestridx = 0;\n");
        ;
        (void)fprintf(stdout, "while(");
        ;
        __writestridx = 0;
        while (token[__writestridx] != '\0' && __writestridx < 256)
          (void)fputc((unsigned char)token[__writestridx++], stdout);
        ;
        (void)fprintf(stdout, "[__writestridx]!=\'\\0\'&&__writestridx<");
        ;
        (void)fprintf(stdout, "%ld", (long)(symtab[save + 34]));
        ;
        (void)fprintf(stdout, ")\n");
        ;
        (void)fprintf(stdout, "(void)fputc((unsigned char)");
        ;
        __writestridx = 0;
        while (token[__writestridx] != '\0' && __writestridx < 256)
          (void)fputc((unsigned char)token[__writestridx++], stdout);
        ;
        (void)fprintf(stdout, "[__writestridx++],stdout);\n");
        ;
      };
    } else {
      {
        (void)fprintf(stdout, "(void)fprintf(stdout, ");
        ;
        __writestridx = 0;
        while (token[__writestridx] != '\0' && __writestridx < 256)
          (void)fputc((unsigned char)token[__writestridx++], stdout);
        ;
        (void)fprintf(stdout, ");\n");
        ;
      };
    };
    ;
  };
}

static void next(void) {
  {
    lex();
    ;
    while (type == -1)
      lex();
    ;
    ;
  };
}

static void syntax(void) {
  {
    error();
    ;
    (void)fprintf(stdout, "syntax error\n");
    ;
    exit(1);
    ;
    ;
  };
}

static void expect_begin(void) {
  {
    if (type != TOK_BEGIN) {
      syntax();
      ;
    };
    next();
    ;
    ;
  };
}

static void expect_call(void) {
  {
    if (type != TOK_CALL) {
      syntax();
      ;
    };
    next();
    ;
    ;
  };
}

static void expect_const(void) {
  {
    if (type != TOK_CONST) {
      syntax();
      ;
    };
    next();
    ;
    ;
  };
}

static void expect_do(void) {
  {
    if (type != TOK_DO) {
      syntax();
      ;
    };
    next();
    ;
    ;
  };
}

static void expect_else(void) {
  {
    if (type != TOK_ELSE) {
      syntax();
      ;
    };
    next();
    ;
    ;
  };
}

static void expect_end(void) {
  {
    if (type != TOK_END) {
      syntax();
      ;
    };
    next();
    ;
    ;
  };
}

static void expect_exit(void) {
  {
    if (type != TOK_EXIT) {
      syntax();
      ;
    };
    next();
    ;
    ;
  };
}

static void expect_forward(void) {
  {
    if (type != TOK_FORWARD) {
      syntax();
      ;
    };
    next();
    ;
    ;
  };
}

static void expect_ident(void) {
  {
    if (type != TOK_IDENT) {
      syntax();
      ;
    };
    next();
    ;
    ;
  };
}

static void expect_if(void) {
  {
    if (type != TOK_IF) {
      syntax();
      ;
    };
    next();
    ;
    ;
  };
}

static void expect_into(void) {
  {
    if (type != TOK_INTO) {
      syntax();
      ;
    };
    next();
    ;
    ;
  };
}

static void expect_number(void) {
  {
    if (type != TOK_NUMBER) {
      syntax();
      ;
    };
    next();
    ;
    ;
  };
}

static void expect_odd(void) {
  {
    if (type != TOK_ODD) {
      syntax();
      ;
    };
    next();
    ;
    ;
  };
}

static void expect_procedure(void) {
  {
    if (type != TOK_PROCEDURE) {
      syntax();
      ;
    };
    next();
    ;
    ;
  };
}

static void expect_readchar(void) {
  {
    if (type != TOK_READCHAR) {
      syntax();
      ;
    };
    next();
    ;
    ;
  };
}

static void expect_readint(void) {
  {
    if (type != TOK_READINT) {
      syntax();
      ;
    };
    next();
    ;
    ;
  };
}

static void expect_size(void) {
  {
    if (type != TOK_SIZE) {
      syntax();
      ;
    };
    next();
    ;
    ;
  };
}

static void expect_string(void) {
  {
    if (type != TOK_STRING) {
      syntax();
      ;
    };
    next();
    ;
    ;
  };
}

static void expect_then(void) {
  {
    if (type != TOK_THEN) {
      syntax();
      ;
    };
    next();
    ;
    ;
  };
}

static void expect_var(void) {
  {
    if (type != TOK_VAR) {
      syntax();
      ;
    };
    next();
    ;
    ;
  };
}

static void expect_while(void) {
  {
    if (type != TOK_WHILE) {
      syntax();
      ;
    };
    next();
    ;
    ;
  };
}

static void expect_writechar(void) {
  {
    if (type != TOK_WRITECHAR) {
      syntax();
      ;
    };
    next();
    ;
    ;
  };
}

static void expect_writeint(void) {
  {
    if (type != TOK_WRITEINT) {
      syntax();
      ;
    };
    next();
    ;
    ;
  };
}

static void expect_writestr(void) {
  {
    if (type != TOK_WRITESTR) {
      syntax();
      ;
    };
    next();
    ;
    ;
  };
}

static void assign(void) {
  {
    if (type != TOK_ASSIGN) {
      syntax();
      ;
    };
    next();
    ;
    ;
  };
}

static void comma(void) {
  {
    if (type != TOK_COMMA) {
      syntax();
      ;
    };
    next();
    ;
    ;
  };
}

static void dot(void) {
  {
    if (type != TOK_DOT) {
      syntax();
      ;
    };
    next();
    ;
    ;
  };
}

static void equal(void) {
  {
    if (type != TOK_EQUAL) {
      syntax();
      ;
    };
    next();
    ;
    ;
  };
}

static void lbrack(void) {
  {
    if (type != TOK_LBRACK) {
      syntax();
      ;
    };
    next();
    ;
    ;
  };
}

static void lparen(void) {
  {
    if (type != TOK_LPAREN) {
      syntax();
      ;
    };
    next();
    ;
    ;
  };
}

static void rbrack(void) {
  {
    if (type != TOK_RBRACK) {
      syntax();
      ;
    };
    next();
    ;
    ;
  };
}

static void rparen(void) {
  {
    if (type != TOK_RPAREN) {
      syntax();
      ;
    };
    next();
    ;
    ;
  };
}

static void semicolon(void) {
  {
    if (type != TOK_SEMICOLON) {
      syntax();
      ;
    };
    next();
    ;
    ;
  };
}

static void addsymbol(void) {
  long i;
  long n;

  {
    i = symtabcnt * 35;
    n = 0;
    while (n < 32) {
      symtab[i] = token[n];
      i = i + 1;
      n = n + 1;
    };
    symtab[i] = symtype;
    i = i + 1;
    symtab[i] = depth - 1;
    i = i + 1;
    symtab[i] = 0;
    symtabcnt = symtabcnt + 1;
    if (symtabcnt == 29961) {
      {
        error();
        ;
        (void)fprintf(stdout, "symbol table exhausted\n");
        ;
        exit(1);
        ;
        ;
      };
    };
    ;
  };
}

static void arraycheck(void) {
  long i;
  long n;
  long p;
  long save;

  {
    i = 0;
    save = 0;
    while (i < symtabcnt) {
      clrstr();
      ;
      n = 0;
      p = 35 * i;
      while (n < 32) {
        str[n] = symtab[p];
        p = p + 1;
        n = n + 1;
      };
      cmpstr();
      ;
      if (ret == 0) {
        save = 35 * i;
      };
      i = i + 1;
    };
    if (save == 0) {
      {
        error();
        ;
        (void)fprintf(stdout, "undefined symbol: \'");
        ;
        __writestridx = 0;
        while (token[__writestridx] != '\0' && __writestridx < 256)
          (void)fputc((unsigned char)token[__writestridx++], stdout);
        ;
        (void)fprintf(stdout, "\'\n");
        ;
        exit(1);
        ;
        ;
      };
    };
    if (symtab[save + 34] == 0) {
      {
        error();
        ;
        (void)fprintf(stdout, "not an array: \'");
        ;
        __writestridx = 0;
        while (token[__writestridx] != '\0' && __writestridx < 256)
          (void)fputc((unsigned char)token[__writestridx++], stdout);
        ;
        (void)fprintf(stdout, "\'\n");
        ;
        exit(1);
        ;
        ;
      };
    };
    ;
  };
}

static void arraysize(void) {
  long i;

  {
    i = 0;
    i = 35 * (symtabcnt - 1);
    if (symtab[i + 32] != TOK_VAR) {
      {
        error();
        ;
        (void)fprintf(stdout, "arrays must be declared with \"var\": \'");
        ;
        __writestridx = 0;
        while (token[__writestridx] != '\0' && __writestridx < 256)
          (void)fputc((unsigned char)token[__writestridx++], stdout);
        ;
        (void)fprintf(stdout, "\'\n");
        ;
        exit(1);
        ;
        ;
      };
    };
    symtab[i + 34] = value;
    if (value < 1) {
      {
        error();
        ;
        (void)fprintf(stdout, "invalid array size: \'");
        ;
        __writestridx = 0;
        while (token[__writestridx] != '\0' && __writestridx < 256)
          (void)fputc((unsigned char)token[__writestridx++], stdout);
        ;
        (void)fprintf(stdout, "\'\n");
        ;
        exit(1);
        ;
        ;
      };
    };
    ;
  };
}

static void initsymtab(void) {
  long i;

  {
    symtab[0] = 'm';
    symtab[1] = 'a';
    symtab[2] = 'i';
    symtab[3] = 'n';
    i = 4;
    while (i < 32) {
      symtab[i] = '\0';
      i = i + 1;
    };
    symtab[i] = TOK_PROCEDURE;
    i = i + 1;
    symtab[i] = 0;
    i = i + 1;
    symtab[i] = 0;
    symtabcnt = 1;
  };
}

static void symcheck(void) {
  long i;
  long n;
  long p;
  long save;

  {
    i = 0;
    save = 0;
    while (i < symtabcnt) {
      clrstr();
      ;
      n = 0;
      p = 35 * i;
      while (n < 32) {
        str[n] = symtab[p];
        p = p + 1;
        n = n + 1;
      };
      cmpstr();
      ;
      if (ret == 0) {
        save = 35 * i;
      };
      i = i + 1;
    };
    if (save == 0) {
      {
        error();
        ;
        (void)fprintf(stdout, "undefined symbol: \'");
        ;
        __writestridx = 0;
        while (token[__writestridx] != '\0' && __writestridx < 256)
          (void)fputc((unsigned char)token[__writestridx++], stdout);
        ;
        (void)fprintf(stdout, "\'\n");
        ;
        exit(1);
        ;
        ;
      };
    };
    if (symtype == CHECK_LHS) {
      {
        if (symtab[save + 32] != TOK_VAR) {
          {
            error();
            ;
            (void)fprintf(stdout, "must be a variable: \'");
            ;
            __writestridx = 0;
            while (token[__writestridx] != '\0' && __writestridx < 256)
              (void)fputc((unsigned char)token[__writestridx++], stdout);
            ;
            (void)fprintf(stdout, "\'\n");
            ;
            exit(1);
            ;
            ;
          };
        };
        ;
      };
    } else {
      if (symtype == CHECK_RHS) {
        {
          if (symtab[save + 32] == TOK_PROCEDURE) {
            {
              error();
              ;
              (void)fprintf(stdout, "must not be a procedure: \'");
              ;
              __writestridx = 0;
              while (token[__writestridx] != '\0' && __writestridx < 256)
                (void)fputc((unsigned char)token[__writestridx++], stdout);
              ;
              (void)fprintf(stdout, "\'\n");
              ;
              exit(1);
              ;
              ;
            };
          } else {
            if (symtab[save + 32] == TOK_FORWARD) {
              {
                error();
                ;
                (void)fprintf(stdout, "must not be a procedure: \'");
                ;
                __writestridx = 0;
                while (token[__writestridx] != '\0' && __writestridx < 256)
                  (void)fputc((unsigned char)token[__writestridx++], stdout);
                ;
                (void)fprintf(stdout, "\'\n");
                ;
                exit(1);
                ;
                ;
              };
            };
          };
          ;
        };
      } else {
        if (symtype == CHECK_CALL) {
          {
            if (symtab[save + 32] != TOK_PROCEDURE) {
              {
                if (symtab[save + 32] != TOK_FORWARD) {
                  {
                    error();
                    ;
                    (void)fprintf(stdout, "must be a procedure: \'");
                    ;
                    __writestridx = 0;
                    while (token[__writestridx] != '\0' && __writestridx < 256)
                      (void)fputc((unsigned char)token[__writestridx++],
                                  stdout);
                    ;
                    (void)fprintf(stdout, "\'\n");
                    ;
                    exit(1);
                    ;
                    ;
                  };
                };
                ;
              };
            };
            ;
          };
        };
      };
    };
    ;
  };
}

static void destroysymbols(void) {
  long i;
  long n;

  {
    i = 35 * (symtabcnt - 1);
    while (symtab[i + 32] != TOK_PROCEDURE) {
      n = 0;
      while (n < 35) {
        symtab[i] = '\0';
        i = i + 1;
        n = n + 1;
      };
      symtabcnt = symtabcnt - 1;
      i = 35 * (symtabcnt - 1);
    };
    ;
  };
}

static void factor(void) {
  {
    if (type == TOK_IDENT) {
      {
        symtype = CHECK_RHS;
        symcheck();
        ;
        cg_symbol();
        ;
        expect_ident();
        ;
        if (type == TOK_LBRACK) {
          {
            arraycheck();
            ;
            cg_symbol();
            ;
            lbrack();
            ;
            expression();
            ;
            if (type == TOK_RBRACK) {
              {
                cg_symbol();
                ;
                ;
              };
            };
            rbrack();
            ;
            ;
          };
        };
        ;
      };
    } else {
      if (type == TOK_NUMBER) {
        {
          cg_symbol();
          ;
          expect_number();
          ;
        };
      } else {
        if (type == TOK_LPAREN) {
          {
            cg_symbol();
            ;
            lparen();
            ;
            expression();
            ;
            if (type == TOK_RPAREN) {
              {
                cg_symbol();
                ;
                ;
              };
            };
            rparen();
            ;
            ;
          };
        };
      };
    };
    ;
  };
}

static void term(void) {
  long loop;

  {
    loop = 1;
    factor();
    ;
    while (loop != 0) {
      loop = 0;
      if (type == TOK_MULTIPLY) {
        {
          loop = 1;
          cg_symbol();
          ;
          next();
          ;
          factor();
          ;
          ;
        };
      } else {
        if (type == TOK_DIVIDE) {
          {
            loop = 1;
            cg_symbol();
            ;
            next();
            ;
            factor();
            ;
            ;
          };
        } else {
          if (type == TOK_MODULO) {
            {
              loop = 1;
              cg_symbol();
              ;
              next();
              ;
              factor();
              ;
              ;
            };
          } else {
            if (type == TOK_AND) {
              {
                loop = 1;
                cg_symbol();
                ;
                next();
                ;
                factor();
                ;
                ;
              };
            };
          };
        };
      };
      ;
    };
    ;
  };
}

static void expression(void) {
  long loop;

  {
    loop = 1;
    if (type == TOK_PLUS) {
      {
        cg_symbol();
        ;
        next();
        ;
      };
    } else {
      if (type == TOK_MINUS) {
        {
          cg_symbol();
          ;
          next();
          ;
        };
      };
    };
    term();
    ;
    while (loop != 0) {
      loop = 0;
      if (type == TOK_PLUS) {
        {
          loop = 1;
          cg_symbol();
          ;
          next();
          ;
          term();
          ;
          ;
        };
      } else {
        if (type == TOK_MINUS) {
          {
            loop = 1;
            cg_symbol();
            ;
            next();
            ;
            term();
            ;
            ;
          };
        } else {
          if (type == TOK_OR) {
            {
              loop = 1;
              cg_symbol();
              ;
              next();
              ;
              term();
              ;
              ;
            };
          };
        };
      };
      ;
    };
    ;
  };
}

static void condition(void) {
  {
    if (type == TOK_ODD) {
      {
        cg_symbol();
        ;
        expect_odd();
        ;
        expression();
        ;
        cg_odd();
        ;
      };
    } else {
      {
        expression();
        ;
        if (type == TOK_EQUAL) {
          {
            cg_symbol();
            ;
            next();
            ;
          };
        } else {
          if (type == TOK_HASH) {
            {
              cg_symbol();
              ;
              next();
              ;
            };
          } else {
            if (type == TOK_LTHAN) {
              {
                cg_symbol();
                ;
                next();
                ;
              };
            } else {
              if (type == TOK_GTHAN) {
                {
                  cg_symbol();
                  ;
                  next();
                  ;
                };
              } else {
                if (type == TOK_LTHANE) {
                  {
                    cg_symbol();
                    ;
                    next();
                    ;
                  };
                } else {
                  if (type == TOK_GTHANE) {
                    {
                      cg_symbol();
                      ;
                      next();
                      ;
                    };
                  } else {
                    {
                      error();
                      ;
                      (void)fprintf(stdout, "invalid conditional: \'");
                      ;
                      __writestridx = 0;
                      while (token[__writestridx] != '\0' &&
                             __writestridx < 256)
                        (void)fputc((unsigned char)token[__writestridx++],
                                    stdout);
                      ;
                      (void)fprintf(stdout, "\'\n");
                      ;
                      exit(1);
                      ;
                      ;
                    };
                  };
                };
              };
            };
          };
        };
        expression();
        ;
        ;
      };
    };
    ;
  };
}

static void statement(void) {
  {
    if (type == TOK_IDENT) {
      {
        symtype = CHECK_LHS;
        symcheck();
        ;
        cg_symbol();
        ;
        expect_ident();
        ;
        if (type == TOK_LBRACK) {
          {
            arraycheck();
            ;
            cg_symbol();
            ;
            lbrack();
            ;
            expression();
            ;
            if (type == TOK_RBRACK) {
              {
                cg_symbol();
                ;
                ;
              };
            };
            rbrack();
            ;
            ;
          };
        };
        if (type == TOK_ASSIGN) {
          {
            cg_symbol();
            ;
            ;
          };
        };
        assign();
        ;
        expression();
        ;
        ;
      };
    } else {
      if (type == TOK_CALL) {
        {
          expect_call();
          ;
          if (type == TOK_IDENT) {
            {
              symtype = TOK_CALL;
              symcheck();
              ;
              cg_call();
              ;
            };
          };
          expect_ident();
          ;
          ;
        };
      } else {
        if (type == TOK_BEGIN) {
          {
            cg_symbol();
            ;
            expect_begin();
            ;
            statement();
            ;
            while (type == TOK_SEMICOLON) {
              cg_semicolon();
              ;
              semicolon();
              ;
              statement();
              ;
            };
            if (type == TOK_END) {
              {
                cg_symbol();
                ;
                ;
              };
            };
            expect_end();
            ;
            ;
          };
        } else {
          if (type == TOK_IF) {
            {
              cg_symbol();
              ;
              expect_if();
              ;
              condition();
              ;
              if (type == TOK_THEN) {
                {
                  cg_symbol();
                  ;
                  ;
                };
              };
              expect_then();
              ;
              statement();
              ;
              if (type == TOK_ELSE) {
                {
                  cg_symbol();
                  ;
                  expect_else();
                  ;
                  statement();
                  ;
                };
              };
              cg_closeblock();
              ;
            };
          } else {
            if (type == TOK_WHILE) {
              {
                cg_symbol();
                ;
                expect_while();
                ;
                condition();
                ;
                if (type == TOK_DO) {
                  {
                    cg_symbol();
                    ;
                    ;
                  };
                };
                expect_do();
                ;
                statement();
                ;
                ;
              };
            } else {
              if (type == TOK_WRITEINT) {
                {
                  expect_writeint();
                  ;
                  cg_writeint();
                  ;
                  expression();
                  ;
                  cg_rparen();
                  ;
                  cg_rparen();
                  ;
                  cg_semicolon();
                  ;
                  ;
                };
              } else {
                if (type == TOK_WRITECHAR) {
                  {
                    expect_writechar();
                    ;
                    cg_writechar();
                    ;
                    expression();
                    ;
                    cg_rparen();
                    ;
                    cg_rparen();
                    ;
                    cg_semicolon();
                    ;
                    ;
                  };
                } else {
                  if (type == TOK_WRITESTR) {
                    {
                      expect_writestr();
                      ;
                      if (type == TOK_IDENT) {
                        {
                          symtype = CHECK_LHS;
                          symcheck();
                          ;
                          cg_writestr();
                          ;
                          expect_ident();
                          ;
                        };
                      } else {
                        if (type == TOK_STRING) {
                          {
                            cg_writestr();
                            ;
                            expect_string();
                            ;
                          };
                        } else {
                          {
                            error();
                            ;
                            (void)fprintf(
                                stdout,
                                "writeStr takes an array or a string: \'");
                            ;
                            __writestridx = 0;
                            while (token[__writestridx] != '\0' &&
                                   __writestridx < 256)
                              (void)fputc((unsigned char)token[__writestridx++],
                                          stdout);
                            ;
                            (void)fprintf(stdout, "\'\n");
                            ;
                            exit(1);
                            ;
                            ;
                          };
                        };
                      };
                      ;
                    };
                  } else {
                    if (type == TOK_READINT) {
                      {
                        expect_readint();
                        ;
                        if (type == TOK_INTO) {
                          expect_into();
                          ;
                        };
                        if (type == TOK_IDENT) {
                          {
                            symtype = CHECK_LHS;
                            symcheck();
                            ;
                            cg_readint();
                            ;
                          };
                        };
                        expect_ident();
                        ;
                        ;
                      };
                    } else {
                      if (type == TOK_READCHAR) {
                        {
                          expect_readchar();
                          ;
                          if (type == TOK_INTO) {
                            expect_into();
                            ;
                          };
                          if (type == TOK_IDENT) {
                            {
                              symtype = CHECK_LHS;
                              symcheck();
                              ;
                              cg_readchar();
                              ;
                            };
                          };
                          expect_ident();
                          ;
                          ;
                        };
                      } else {
                        if (type == TOK_EXIT) {
                          {
                            expect_exit();
                            ;
                            cg_exit();
                            ;
                            expression();
                            ;
                            cg_rparen();
                            ;
                            cg_semicolon();
                            ;
                          };
                        };
                      };
                    };
                  };
                };
              };
            };
          };
        };
      };
    };
    ;
  };
}

static void block(void) {
  {
    if (depth > 1) {
      {
        error();
        ;
        (void)fprintf(stdout, "nesting depth exceeded\n");
        ;
        exit(1);
        ;
        ;
      };
    };
    depth = depth + 1;
    if (type == TOK_CONST) {
      {
        expect_const();
        ;
        if (type == TOK_IDENT) {
          {
            symtype = TOK_CONST;
            addsymbol();
            ;
            cg_const();
            ;
          };
        };
        expect_ident();
        ;
        equal();
        ;
        if (type == TOK_NUMBER) {
          {
            cg_symbol();
            ;
            cg_semicolon();
            ;
          };
        };
        expect_number();
        ;
        while (type == TOK_COMMA) {
          comma();
          ;
          if (type == TOK_IDENT) {
            {
              symtype = TOK_CONST;
              addsymbol();
              ;
              cg_const();
              ;
            };
          };
          expect_ident();
          ;
          equal();
          ;
          if (type == TOK_NUMBER) {
            {
              cg_symbol();
              ;
              cg_semicolon();
              ;
            };
          };
          expect_number();
          ;
          ;
        };
        semicolon();
        ;
        ;
      };
    };
    if (type == TOK_VAR) {
      {
        expect_var();
        ;
        if (type == TOK_IDENT) {
          {
            symtype = TOK_VAR;
            addsymbol();
            ;
            cg_var();
            ;
          };
        };
        expect_ident();
        ;
        if (type == TOK_SIZE) {
          {
            expect_size();
            ;
            if (type == TOK_NUMBER) {
              {
                arraysize();
                ;
                cg_array();
                ;
                ;
              };
            };
            expect_number();
            ;
            ;
          };
        };
        cg_semicolon();
        ;
        while (type == TOK_COMMA) {
          comma();
          ;
          if (type == TOK_IDENT) {
            {
              symtype = TOK_VAR;
              addsymbol();
              ;
              cg_var();
              ;
            };
          };
          expect_ident();
          ;
          if (type == TOK_SIZE) {
            {
              expect_size();
              ;
              if (type == TOK_NUMBER) {
                {
                  arraysize();
                  ;
                  cg_array();
                  ;
                  expect_number();
                  ;
                };
              };
              ;
            };
          };
          cg_semicolon();
          ;
        };
        semicolon();
        ;
        cg_crlf();
        ;
      };
    };
    while (type == TOK_FORWARD) {
      expect_forward();
      ;
      if (type == TOK_IDENT) {
        {
          symtype = TOK_FORWARD;
          addsymbol();
          ;
          cg_forward();
          ;
        };
      };
      expect_ident();
      ;
      semicolon();
      ;
      ;
    };
    while (type == TOK_PROCEDURE) {
      proc = 1;
      expect_procedure();
      ;
      if (type == TOK_IDENT) {
        {
          symtype = TOK_PROCEDURE;
          addsymbol();
          ;
          cg_procedure();
          ;
        };
      };
      expect_ident();
      ;
      semicolon();
      ;
      block();
      ;
      semicolon();
      ;
      proc = 0;
      destroysymbols();
      ;
      ;
    };
    if (proc == 0) {
      {
        cg_procedure();
        ;
        ;
      };
    };
    statement();
    ;
    cg_epilogue();
    ;
    depth = depth - 1;
    if (depth < 0) {
      {
        error();
        ;
        (void)fprintf(stdout, "nesting depth fell below 0\n");
        ;
        exit(1);
        ;
        ;
      };
    };
    ;
  };
}

static void parse(void) {
  {
    cg_init();
    ;
    next();
    ;
    block();
    ;
    dot();
    ;
    if (type != 0) {
      {
        error();
        ;
        (void)fprintf(stdout, "extra tokens at end of file: \'");
        ;
        __writestridx = 0;
        while (token[__writestridx] != '\0' && __writestridx < 256)
          (void)fputc((unsigned char)token[__writestridx++], stdout);
        ;
        (void)fprintf(stdout, "\'\n");
        ;
        exit(1);
        ;
        ;
      };
    };
    cg_end();
    ;
    ;
  };
}

int main(int argc, char *argv[]) {
  {
    line = 1;
    type = -1;
    setupkeywords();
    ;
    readin();
    ;
    initsymtab();
    ;
    parse();
    ;
  };
  return 0;
}

/* PL/0 compiler 1.0.0 */
