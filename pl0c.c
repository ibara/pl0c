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
static long errstr[35];
static long token[256];
static long type;
static long str[256];
static long symtabcnt;
static long ret;
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
    __writestridx = 0;
    while (errstr[__writestridx] != '\0' && __writestridx < 35)
      (void)fputc((unsigned char)errstr[__writestridx++], stdout);
    ;
    (void)fprintf(stdout, ": ");
    ;
    __writestridx = 0;
    while (token[__writestridx] != '\0' && __writestridx < 256)
      (void)fputc((unsigned char)token[__writestridx++], stdout);
    ;
    (void)fprintf(stdout, "%c", (unsigned char)('\n'));
    ;
    exit(1);
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
          errstr[0] = 'f';
          errstr[1] = 'i';
          errstr[2] = 'l';
          errstr[3] = 'e';
          errstr[4] = ' ';
          errstr[5] = 't';
          errstr[6] = 'o';
          errstr[7] = 'o';
          errstr[8] = ' ';
          errstr[9] = 'b';
          errstr[10] = 'i';
          errstr[11] = 'g';
          error();
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
          errstr[0] = 'u';
          errstr[1] = 'n';
          errstr[2] = 't';
          errstr[3] = 'e';
          errstr[4] = 'r';
          errstr[5] = 'm';
          errstr[6] = 'i';
          errstr[7] = 'n';
          errstr[8] = 'a';
          errstr[9] = 't';
          errstr[10] = 'e';
          errstr[11] = 'd';
          errstr[12] = ' ';
          errstr[13] = 'c';
          errstr[14] = 'o';
          errstr[15] = 'm';
          errstr[16] = 'm';
          errstr[17] = 'e';
          errstr[18] = 'n';
          errstr[19] = 't';
          error();
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
              if (i == 256) {
                {
                  errstr[0] = 't';
                  errstr[1] = 'o';
                  errstr[2] = 'k';
                  errstr[3] = 'e';
                  errstr[4] = 'n';
                  errstr[5] = ' ';
                  errstr[6] = 't';
                  errstr[7] = 'o';
                  errstr[8] = 'o';
                  errstr[9] = ' ';
                  errstr[10] = 'l';
                  errstr[11] = 'o';
                  errstr[12] = 'n';
                  errstr[13] = 'g';
                  error();
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
          if (raw[loc] >= 'a') {
            {
              if (raw[loc] <= 'z') {
                {
                  loop = 1;
                  token[i] = raw[loc];
                  i = i + 1;
                  if (i == 256) {
                    {
                      errstr[0] = 't';
                      errstr[1] = 'o';
                      errstr[2] = 'k';
                      errstr[3] = 'e';
                      errstr[4] = 'n';
                      errstr[5] = ' ';
                      errstr[6] = 't';
                      errstr[7] = 'o';
                      errstr[8] = 'o';
                      errstr[9] = ' ';
                      errstr[10] = 'l';
                      errstr[11] = 'o';
                      errstr[12] = 'n';
                      errstr[13] = 'g';
                      error();
                      ;
                    };
                  };
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
                  if (i == 256) {
                    {
                      errstr[0] = 't';
                      errstr[1] = 'o';
                      errstr[2] = 'k';
                      errstr[3] = 'e';
                      errstr[4] = 'n';
                      errstr[5] = ' ';
                      errstr[6] = 't';
                      errstr[7] = 'o';
                      errstr[8] = 'o';
                      errstr[9] = ' ';
                      errstr[10] = 'l';
                      errstr[11] = 'o';
                      errstr[12] = 'n';
                      errstr[13] = 'g';
                      error();
                      ;
                    };
                  };
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
              if (i == 256) {
                {
                  errstr[0] = 't';
                  errstr[1] = 'o';
                  errstr[2] = 'k';
                  errstr[3] = 'e';
                  errstr[4] = 'n';
                  errstr[5] = ' ';
                  errstr[6] = 't';
                  errstr[7] = 'o';
                  errstr[8] = 'o';
                  errstr[9] = ' ';
                  errstr[10] = 'l';
                  errstr[11] = 'o';
                  errstr[12] = 'n';
                  errstr[13] = 'g';
                  error();
                  ;
                };
              };
              ;
            };
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
    clrstr();
    ;
    str[0] = 'c';
    str[1] = 'o';
    str[2] = 'n';
    str[3] = 's';
    str[4] = 't';
    cmpstr();
    ;
    if (ret == 0) {
      type = TOK_CONST;
    };
    if (ret != 0) {
      {
        clrstr();
        ;
        str[0] = 'v';
        str[1] = 'a';
        str[2] = 'r';
        cmpstr();
        ;
        if (ret == 0) {
          type = TOK_VAR;
        };
        ;
      };
    };
    if (ret != 0) {
      {
        clrstr();
        ;
        str[0] = 'p';
        str[1] = 'r';
        str[2] = 'o';
        str[3] = 'c';
        str[4] = 'e';
        str[5] = 'd';
        str[6] = 'u';
        str[7] = 'r';
        str[8] = 'e';
        cmpstr();
        ;
        if (ret == 0) {
          type = TOK_PROCEDURE;
        };
        ;
      };
    };
    if (ret != 0) {
      {
        clrstr();
        ;
        str[0] = 'c';
        str[1] = 'a';
        str[2] = 'l';
        str[3] = 'l';
        cmpstr();
        ;
        if (ret == 0) {
          type = TOK_CALL;
        };
        ;
      };
    };
    if (ret != 0) {
      {
        clrstr();
        ;
        str[0] = 'b';
        str[1] = 'e';
        str[2] = 'g';
        str[3] = 'i';
        str[4] = 'n';
        cmpstr();
        ;
        if (ret == 0) {
          type = TOK_BEGIN;
        };
        ;
      };
    };
    if (ret != 0) {
      {
        clrstr();
        ;
        str[0] = 'e';
        str[1] = 'n';
        str[2] = 'd';
        cmpstr();
        ;
        if (ret == 0) {
          type = TOK_END;
        };
        ;
      };
    };
    if (ret != 0) {
      {
        clrstr();
        ;
        str[0] = 'i';
        str[1] = 'f';
        cmpstr();
        ;
        if (ret == 0) {
          type = TOK_IF;
        };
        ;
      };
    };
    if (ret != 0) {
      {
        clrstr();
        ;
        str[0] = 't';
        str[1] = 'h';
        str[2] = 'e';
        str[3] = 'n';
        cmpstr();
        ;
        if (ret == 0) {
          type = TOK_THEN;
        };
        ;
      };
    };
    if (ret != 0) {
      {
        clrstr();
        ;
        str[0] = 'e';
        str[1] = 'l';
        str[2] = 's';
        str[3] = 'e';
        cmpstr();
        ;
        if (ret == 0) {
          type = TOK_ELSE;
        };
        ;
      };
    };
    if (ret != 0) {
      {
        clrstr();
        ;
        str[0] = 'w';
        str[1] = 'h';
        str[2] = 'i';
        str[3] = 'l';
        str[4] = 'e';
        cmpstr();
        ;
        if (ret == 0) {
          type = TOK_WHILE;
        };
        ;
      };
    };
    if (ret != 0) {
      {
        clrstr();
        ;
        str[0] = 'd';
        str[1] = 'o';
        cmpstr();
        ;
        if (ret == 0) {
          type = TOK_DO;
        };
        ;
      };
    };
    if (ret != 0) {
      {
        clrstr();
        ;
        str[0] = 'o';
        str[1] = 'd';
        str[2] = 'd';
        cmpstr();
        ;
        if (ret == 0) {
          type = TOK_ODD;
        };
        ;
      };
    };
    if (ret != 0) {
      {
        clrstr();
        ;
        str[0] = 'w';
        str[1] = 'r';
        str[2] = 'i';
        str[3] = 't';
        str[4] = 'e';
        str[5] = 'I';
        str[6] = 'n';
        str[7] = 't';
        cmpstr();
        ;
        if (ret == 0) {
          type = TOK_WRITEINT;
        };
        ;
      };
    };
    if (ret != 0) {
      {
        clrstr();
        ;
        str[0] = 'w';
        str[1] = 'r';
        str[2] = 'i';
        str[3] = 't';
        str[4] = 'e';
        str[5] = 'C';
        str[6] = 'h';
        str[7] = 'a';
        str[8] = 'r';
        cmpstr();
        ;
        if (ret == 0) {
          type = TOK_WRITECHAR;
        };
        ;
      };
    };
    if (ret != 0) {
      {
        clrstr();
        ;
        str[0] = 'w';
        str[1] = 'r';
        str[2] = 'i';
        str[3] = 't';
        str[4] = 'e';
        str[5] = 'S';
        str[6] = 't';
        str[7] = 'r';
        cmpstr();
        ;
        if (ret == 0) {
          type = TOK_WRITESTR;
        };
        ;
      };
    };
    if (ret != 0) {
      {
        clrstr();
        ;
        str[0] = 'r';
        str[1] = 'e';
        str[2] = 'a';
        str[3] = 'd';
        str[4] = 'I';
        str[5] = 'n';
        str[6] = 't';
        cmpstr();
        ;
        if (ret == 0) {
          type = TOK_READINT;
        };
        ;
      };
    };
    if (ret != 0) {
      {
        clrstr();
        ;
        str[0] = 'r';
        str[1] = 'e';
        str[2] = 'a';
        str[3] = 'd';
        str[4] = 'C';
        str[5] = 'h';
        str[6] = 'a';
        str[7] = 'r';
        cmpstr();
        ;
        if (ret == 0) {
          type = TOK_READCHAR;
        };
        ;
      };
    };
    if (ret != 0) {
      {
        clrstr();
        ;
        str[0] = 'i';
        str[1] = 'n';
        str[2] = 't';
        str[3] = 'o';
        cmpstr();
        ;
        if (ret == 0) {
          type = TOK_INTO;
        };
        ;
      };
    };
    if (ret != 0) {
      {
        clrstr();
        ;
        str[0] = 's';
        str[1] = 'i';
        str[2] = 'z';
        str[3] = 'e';
        cmpstr();
        ;
        if (ret == 0) {
          type = TOK_SIZE;
        };
        ;
      };
    };
    if (ret != 0) {
      {
        clrstr();
        ;
        str[0] = 'e';
        str[1] = 'x';
        str[2] = 'i';
        str[3] = 't';
        cmpstr();
        ;
        if (ret == 0) {
          type = TOK_EXIT;
        };
        ;
      };
    };
    if (ret != 0) {
      {
        clrstr();
        ;
        str[0] = 'a';
        str[1] = 'n';
        str[2] = 'd';
        cmpstr();
        ;
        if (ret == 0) {
          type = TOK_AND;
        };
        ;
      };
    };
    if (ret != 0) {
      {
        clrstr();
        ;
        str[0] = 'o';
        str[1] = 'r';
        cmpstr();
        ;
        if (ret == 0) {
          type = TOK_OR;
        };
        ;
      };
    };
    if (ret != 0) {
      {
        clrstr();
        ;
        str[0] = 'n';
        str[1] = 'o';
        str[2] = 't';
        cmpstr();
        ;
        if (ret == 0) {
          type = TOK_NOT;
        };
        ;
      };
    };
    if (ret != 0) {
      {
        clrstr();
        ;
        str[0] = 'm';
        str[1] = 'o';
        str[2] = 'd';
        cmpstr();
        ;
        if (ret == 0) {
          type = TOK_MODULO;
        };
        ;
      };
    };
    if (ret != 0) {
      {
        clrstr();
        ;
        str[0] = 'f';
        str[1] = 'o';
        str[2] = 'r';
        str[3] = 'w';
        str[4] = 'a';
        str[5] = 'r';
        str[6] = 'd';
        cmpstr();
        ;
        if (ret == 0) {
          type = TOK_FORWARD;
        };
        ;
      };
    };
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
                  errstr[0] = 't';
                  errstr[1] = 'o';
                  errstr[2] = 'k';
                  errstr[3] = 'e';
                  errstr[4] = 'n';
                  errstr[5] = ' ';
                  errstr[6] = 't';
                  errstr[7] = 'o';
                  errstr[8] = 'o';
                  errstr[9] = ' ';
                  errstr[10] = 'l';
                  errstr[11] = 'o';
                  errstr[12] = 'n';
                  errstr[13] = 'g';
                  error();
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
            errstr[0] = 'u';
            errstr[1] = 'n';
            errstr[2] = 't';
            errstr[3] = 'e';
            errstr[4] = 'r';
            errstr[5] = 'm';
            errstr[6] = 'i';
            errstr[7] = 'n';
            errstr[8] = 'a';
            errstr[9] = 't';
            errstr[10] = 'e';
            errstr[11] = 'd';
            errstr[12] = ' ';
            errstr[13] = 's';
            errstr[14] = 't';
            errstr[15] = 'r';
            errstr[16] = 'i';
            errstr[17] = 'n';
            errstr[18] = 'g';
            error();
            ;
          };
        } else {
          if (raw[loc] == '\0') {
            {
              errstr[0] = 'u';
              errstr[1] = 'n';
              errstr[2] = 't';
              errstr[3] = 'e';
              errstr[4] = 'r';
              errstr[5] = 'm';
              errstr[6] = 'i';
              errstr[7] = 'n';
              errstr[8] = 'a';
              errstr[9] = 't';
              errstr[10] = 'e';
              errstr[11] = 'd';
              errstr[12] = ' ';
              errstr[13] = 's';
              errstr[14] = 't';
              errstr[15] = 'r';
              errstr[16] = 'i';
              errstr[17] = 'n';
              errstr[18] = 'g';
              error();
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
          errstr[0] = 's';
          errstr[1] = 't';
          errstr[2] = 'r';
          errstr[3] = 'i';
          errstr[4] = 'n';
          errstr[5] = 'g';
          errstr[6] = ' ';
          errstr[7] = 't';
          errstr[8] = 'o';
          errstr[9] = 'o';
          errstr[10] = ' ';
          errstr[11] = 'l';
          errstr[12] = 'o';
          errstr[13] = 'n';
          errstr[14] = 'g';
          error();
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
                                                      errstr[0] = 'u';
                                                      errstr[1] = 'n';
                                                      errstr[2] = 'k';
                                                      errstr[3] = 'n';
                                                      errstr[4] = 'o';
                                                      errstr[5] = 'w';
                                                      errstr[6] = 'n';
                                                      errstr[7] = ' ';
                                                      errstr[8] = 't';
                                                      errstr[9] = 'o';
                                                      errstr[10] = 'k';
                                                      errstr[11] = 'e';
                                                      errstr[12] = 'n';
                                                      errstr[13] = ':';
                                                      errstr[14] = ' ';
                                                      errstr[15] = '\'';
                                                      errstr[16] = ':';
                                                      errstr[17] = raw[loc];
                                                      errstr[18] = '\'';
                                                      error();
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
                                                      errstr[0] = 'u';
                                                      errstr[1] = 'n';
                                                      errstr[2] = 'k';
                                                      errstr[3] = 'n';
                                                      errstr[4] = 'o';
                                                      errstr[5] = 'w';
                                                      errstr[6] = 'n';
                                                      errstr[7] = ' ';
                                                      errstr[8] = 't';
                                                      errstr[9] = 'o';
                                                      errstr[10] = 'k';
                                                      errstr[11] = 'e';
                                                      errstr[12] = 'n';
                                                      errstr[13] = ':';
                                                      errstr[14] = ' ';
                                                      errstr[15] = '\'';
                                                      errstr[16] = raw[loc];
                                                      errstr[17] = '\'';
                                                      error();
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
            errstr[0] = 'u';
            errstr[1] = 'n';
            errstr[2] = 'd';
            errstr[3] = 'e';
            errstr[4] = 'f';
            errstr[5] = 'i';
            errstr[6] = 'n';
            errstr[7] = 'e';
            errstr[8] = 'd';
            errstr[9] = ' ';
            errstr[10] = 's';
            errstr[11] = 'y';
            errstr[12] = 'm';
            errstr[13] = 'b';
            errstr[14] = 'o';
            errstr[15] = 'l';
            error();
            ;
          };
        };
        if (symtab[save + 34] == 0) {
          {
            errstr[0] = 'w';
            errstr[1] = 'r';
            errstr[2] = 'i';
            errstr[3] = 't';
            errstr[4] = 'e';
            errstr[5] = 'S';
            errstr[6] = 't';
            errstr[7] = 'r';
            errstr[8] = ' ';
            errstr[9] = 'r';
            errstr[10] = 'e';
            errstr[11] = 'q';
            errstr[12] = 'u';
            errstr[13] = 'i';
            errstr[14] = 'r';
            errstr[15] = 'e';
            errstr[16] = 's';
            errstr[17] = ' ';
            errstr[18] = 'a';
            errstr[19] = 'n';
            errstr[20] = ' ';
            errstr[21] = 'a';
            errstr[22] = 'r';
            errstr[23] = 'r';
            errstr[24] = 'a';
            errstr[25] = 'y';
            error();
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
    errstr[0] = 's';
    errstr[1] = 'y';
    errstr[2] = 'n';
    errstr[3] = 't';
    errstr[4] = 'a';
    errstr[5] = 'x';
    errstr[6] = ' ';
    errstr[7] = 'e';
    errstr[8] = 'r';
    errstr[9] = 'r';
    errstr[10] = 'o';
    errstr[11] = 'r';
    error();
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
        errstr[0] = 's';
        errstr[1] = 'y';
        errstr[2] = 'm';
        errstr[3] = 'b';
        errstr[4] = 'o';
        errstr[5] = 'l';
        errstr[6] = ' ';
        errstr[7] = 't';
        errstr[8] = 'a';
        errstr[9] = 'b';
        errstr[10] = 'l';
        errstr[11] = 'e';
        errstr[12] = ' ';
        errstr[13] = 'e';
        errstr[14] = 'x';
        errstr[15] = 'h';
        errstr[16] = 'a';
        errstr[17] = 'u';
        errstr[18] = 's';
        errstr[19] = 't';
        errstr[20] = 'e';
        errstr[21] = 'd';
        error();
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
        errstr[0] = 'u';
        errstr[1] = 'n';
        errstr[2] = 'd';
        errstr[3] = 'e';
        errstr[4] = 'f';
        errstr[5] = 'i';
        errstr[6] = 'n';
        errstr[7] = 'e';
        errstr[8] = 'd';
        errstr[9] = ' ';
        errstr[10] = 's';
        errstr[11] = 'y';
        errstr[12] = 'm';
        errstr[13] = 'b';
        errstr[14] = 'o';
        errstr[15] = 'l';
        error();
        ;
      };
    };
    if (symtab[save + 34] == 0) {
      {
        errstr[0] = 'n';
        errstr[1] = 'o';
        errstr[2] = 't';
        errstr[3] = ' ';
        errstr[4] = 'a';
        errstr[5] = 'n';
        errstr[6] = ' ';
        errstr[7] = 'a';
        errstr[8] = 'r';
        errstr[9] = 'r';
        errstr[10] = 'a';
        errstr[11] = 'y';
        error();
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
        errstr[0] = 'a';
        errstr[1] = 'r';
        errstr[2] = 'r';
        errstr[3] = 'a';
        errstr[4] = 'y';
        errstr[5] = 's';
        errstr[6] = ' ';
        errstr[7] = 'm';
        errstr[8] = 'u';
        errstr[9] = 's';
        errstr[10] = 't';
        errstr[11] = ' ';
        errstr[12] = 'b';
        errstr[13] = 'e';
        errstr[14] = ' ';
        errstr[15] = 'd';
        errstr[16] = 'e';
        errstr[17] = 'c';
        errstr[18] = 'l';
        errstr[19] = 'a';
        errstr[20] = 'r';
        errstr[21] = 'e';
        errstr[22] = 'd';
        errstr[23] = ' ';
        errstr[24] = 'w';
        errstr[25] = 'i';
        errstr[26] = 't';
        errstr[27] = 'h';
        errstr[28] = ' ';
        errstr[29] = '"';
        errstr[30] = 'v';
        errstr[31] = 'a';
        errstr[32] = 'r';
        errstr[33] = '"';
        error();
        ;
      };
    };
    symtab[i + 34] = value;
    if (value < 1) {
      {
        errstr[0] = 'i';
        errstr[1] = 'n';
        errstr[2] = 'v';
        errstr[3] = 'a';
        errstr[4] = 'l';
        errstr[5] = 'i';
        errstr[6] = 'd';
        errstr[7] = ' ';
        errstr[8] = 'a';
        errstr[9] = 'r';
        errstr[10] = 'r';
        errstr[11] = 'a';
        errstr[12] = 'y';
        errstr[13] = ' ';
        errstr[14] = 's';
        errstr[15] = 'i';
        errstr[16] = 'z';
        errstr[17] = 'e';
        error();
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
        errstr[0] = 'u';
        errstr[1] = 'n';
        errstr[2] = 'd';
        errstr[3] = 'e';
        errstr[4] = 'f';
        errstr[5] = 'i';
        errstr[6] = 'n';
        errstr[7] = 'e';
        errstr[8] = 'd';
        errstr[9] = ' ';
        errstr[10] = 's';
        errstr[11] = 'y';
        errstr[12] = 'm';
        errstr[13] = 'b';
        errstr[14] = 'o';
        errstr[15] = 'l';
        error();
        ;
      };
    };
    if (symtype == CHECK_LHS) {
      {
        if (symtab[save + 32] != TOK_VAR) {
          {
            errstr[0] = 'm';
            errstr[1] = 'u';
            errstr[2] = 's';
            errstr[3] = 't';
            errstr[4] = ' ';
            errstr[5] = 'b';
            errstr[6] = 'e';
            errstr[7] = ' ';
            errstr[8] = 'a';
            errstr[9] = ' ';
            errstr[10] = 'v';
            errstr[11] = 'a';
            errstr[12] = 'r';
            errstr[13] = 'i';
            errstr[14] = 'a';
            errstr[15] = 'b';
            errstr[16] = 'l';
            errstr[17] = 'e';
            error();
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
              errstr[0] = 'm';
              errstr[1] = 'u';
              errstr[2] = 's';
              errstr[3] = 't';
              errstr[4] = ' ';
              errstr[5] = 'n';
              errstr[6] = 'o';
              errstr[7] = 't';
              errstr[8] = ' ';
              errstr[9] = 'b';
              errstr[10] = 'e';
              errstr[11] = ' ';
              errstr[12] = 'a';
              errstr[13] = ' ';
              errstr[14] = 'p';
              errstr[15] = 'r';
              errstr[16] = 'o';
              errstr[17] = 'c';
              errstr[18] = 'e';
              errstr[19] = 'd';
              errstr[20] = 'u';
              errstr[21] = 'r';
              errstr[22] = 'e';
              error();
              ;
            };
          } else {
            if (symtab[save + 32] == TOK_FORWARD) {
              {
                errstr[0] = 'm';
                errstr[1] = 'u';
                errstr[2] = 's';
                errstr[3] = 't';
                errstr[4] = ' ';
                errstr[5] = 'n';
                errstr[6] = 'o';
                errstr[7] = 't';
                errstr[8] = ' ';
                errstr[9] = 'b';
                errstr[10] = 'e';
                errstr[11] = ' ';
                errstr[12] = 'a';
                errstr[13] = ' ';
                errstr[14] = 'p';
                errstr[15] = 'r';
                errstr[16] = 'o';
                errstr[17] = 'c';
                errstr[18] = 'e';
                errstr[19] = 'd';
                errstr[20] = 'u';
                errstr[21] = 'r';
                errstr[22] = 'e';
                error();
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
                    errstr[0] = 'm';
                    errstr[1] = 'u';
                    errstr[2] = 's';
                    errstr[3] = 't';
                    errstr[4] = ' ';
                    errstr[5] = 'b';
                    errstr[6] = 'e';
                    errstr[7] = ' ';
                    errstr[8] = 'a';
                    errstr[9] = ' ';
                    errstr[10] = 'p';
                    errstr[11] = 'r';
                    errstr[12] = 'o';
                    errstr[13] = 'c';
                    errstr[14] = 'e';
                    errstr[15] = 'd';
                    errstr[16] = 'u';
                    errstr[17] = 'r';
                    errstr[18] = 'e';
                    error();
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
                      errstr[0] = 'i';
                      errstr[1] = 'n';
                      errstr[2] = 'v';
                      errstr[3] = 'a';
                      errstr[4] = 'l';
                      errstr[5] = 'i';
                      errstr[6] = 'd';
                      errstr[7] = ' ';
                      errstr[8] = 'c';
                      errstr[9] = 'o';
                      errstr[10] = 'n';
                      errstr[11] = 'd';
                      errstr[12] = 'i';
                      errstr[13] = 't';
                      errstr[14] = 'i';
                      errstr[15] = 'o';
                      errstr[16] = 'n';
                      errstr[17] = 'a';
                      errstr[18] = 'l';
                      error();
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
                            errstr[0] = 'w';
                            errstr[1] = 'r';
                            errstr[2] = 'i';
                            errstr[3] = 't';
                            errstr[4] = 'e';
                            errstr[5] = 'S';
                            errstr[6] = 't';
                            errstr[7] = 'r';
                            errstr[8] = ' ';
                            errstr[9] = 't';
                            errstr[10] = 'a';
                            errstr[11] = 'k';
                            errstr[12] = 'e';
                            errstr[13] = 's';
                            errstr[14] = ' ';
                            errstr[15] = 'a';
                            errstr[16] = 'n';
                            errstr[17] = ' ';
                            errstr[18] = 'a';
                            errstr[19] = 'r';
                            errstr[20] = 'r';
                            errstr[21] = 'a';
                            errstr[22] = 'y';
                            errstr[23] = ' ';
                            errstr[24] = 'o';
                            errstr[25] = 'r';
                            errstr[26] = ' ';
                            errstr[27] = 'a';
                            errstr[28] = ' ';
                            errstr[29] = 's';
                            errstr[30] = 't';
                            errstr[31] = 'r';
                            errstr[32] = 'i';
                            errstr[33] = 'n';
                            errstr[34] = 'g';
                            error();
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
        errstr[0] = 'n';
        errstr[1] = 'e';
        errstr[2] = 's';
        errstr[3] = 't';
        errstr[4] = 'i';
        errstr[5] = 'n';
        errstr[6] = 'g';
        errstr[7] = ' ';
        errstr[8] = 'd';
        errstr[9] = 'e';
        errstr[10] = 'p';
        errstr[11] = 't';
        errstr[12] = 'h';
        errstr[13] = ' ';
        errstr[14] = 'e';
        errstr[15] = 'x';
        errstr[16] = 'c';
        errstr[17] = 'e';
        errstr[18] = 'e';
        errstr[19] = 'd';
        errstr[20] = 'e';
        errstr[21] = 'd';
        error();
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
        errstr[0] = 'n';
        errstr[1] = 'e';
        errstr[2] = 's';
        errstr[3] = 't';
        errstr[4] = 'i';
        errstr[5] = 'n';
        errstr[6] = 'g';
        errstr[7] = ' ';
        errstr[8] = 'd';
        errstr[9] = 'e';
        errstr[10] = 'p';
        errstr[11] = 't';
        errstr[12] = 'h';
        errstr[13] = ' ';
        errstr[14] = 'f';
        errstr[15] = 'e';
        errstr[16] = 'l';
        errstr[17] = 'l';
        errstr[18] = ' ';
        errstr[19] = 'b';
        errstr[20] = 'e';
        errstr[21] = 'l';
        errstr[22] = 'o';
        errstr[23] = 'w';
        errstr[24] = ' ';
        errstr[25] = '0';
        error();
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
        errstr[0] = 'e';
        errstr[1] = 'x';
        errstr[2] = 't';
        errstr[3] = 'r';
        errstr[4] = 'a';
        errstr[5] = ' ';
        errstr[6] = 't';
        errstr[7] = 'o';
        errstr[8] = 'k';
        errstr[9] = 'e';
        errstr[10] = 'n';
        errstr[11] = 's';
        errstr[12] = ' ';
        errstr[13] = 'a';
        errstr[14] = 't';
        errstr[15] = ' ';
        errstr[16] = 'e';
        errstr[17] = 'n';
        errstr[18] = 'd';
        errstr[19] = ' ';
        errstr[20] = 'o';
        errstr[21] = 'f';
        errstr[22] = ' ';
        errstr[23] = 'f';
        errstr[24] = 'i';
        errstr[25] = 'l';
        errstr[26] = 'e';
        error();
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
