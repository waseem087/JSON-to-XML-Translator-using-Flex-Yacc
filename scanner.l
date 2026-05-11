/*
 * scanner.l — Flex lexer for JSON
 * CS-4031 Compiler Construction — Assignment 04
 * Author: Naveed Ahmed (i220889)
 *
 * Recognises all JSON tokens: punctuation, string literals (with
 * escape handling including \uXXXX), numbers (including scientific
 * notation), keywords (true, false, null), and whitespace.
 * Reports lexical errors with line and column information.
 */

%{
#include "ast.h"
#include "parser.tab.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ---- Column tracking -------------------------------------------- */
int yycolumn = 1;

#define YY_USER_ACTION  yycolumn += yyleng;

/* ---- String buffer for building string literals ------------------- */
#define STR_BUF_SIZE 65536
static char string_buf[STR_BUF_SIZE];
static int  string_buf_len;

static void str_buf_reset(void) {
    string_buf_len = 0;
    string_buf[0] = '\0';
}

static void str_buf_append_char(char c) {
    if (string_buf_len < STR_BUF_SIZE - 1) {
        string_buf[string_buf_len++] = c;
        string_buf[string_buf_len]   = '\0';
    }
}

static void str_buf_append(const char *s, int len) {
    for (int i = 0; i < len; i++)
        str_buf_append_char(s[i]);
}

/* Global flag for error state */
extern int json_parse_error;
%}

%option noyywrap
%option yylineno

/* Exclusive start condition for string scanning */
%x STRING

%%

 /* ---- Whitespace (ignored) --------------------------------------- */
[ \t\r]+                { /* skip */ }
\n                      { yycolumn = 1; }

 /* ---- Punctuation ------------------------------------------------ */
"{"                     { return LBRACE; }
"}"                     { return RBRACE; }
"["                     { return LBRACKET; }
"]"                     { return RBRACKET; }
":"                     { return COLON; }
","                     { return COMMA; }

 /* ---- Keywords --------------------------------------------------- */
"true"                  { yylval.bool_val = 1; return T_TRUE; }
"false"                 { yylval.bool_val = 0; return T_FALSE; }
"null"                  { return T_NULL; }

 /* ---- Numbers (with scientific notation support — bonus) ---------- */
-?[0-9]+(\.[0-9]+)?([eE][+-]?[0-9]+)?  {
    yylval.number_val = atof(yytext);
    return NUMBER;
}

 /* ---- String literal (begin) ------------------------------------- */
\"                      { str_buf_reset(); BEGIN(STRING); }

 /* ---- Inside a string -------------------------------------------- */
<STRING>{
    \"                  {
        /* End of string */
        BEGIN(INITIAL);
        yylval.string_val = strdup(string_buf);
        return STRING_LIT;
    }

    \\\"                { str_buf_append_char('"');  }
    \\\\                { str_buf_append_char('\\'); }
    \\\/                { str_buf_append_char('/');  }
    \\b                 { str_buf_append_char('\b'); }
    \\f                 { str_buf_append_char('\f'); }
    \\n                 { str_buf_append_char('\n'); }
    \\r                 { str_buf_append_char('\r'); }
    \\t                 { str_buf_append_char('\t'); }

    /* Unicode escape \uXXXX (bonus feature) */
    \\u[0-9a-fA-F]{4}  {
        unsigned int code;
        sscanf(yytext + 2, "%4x", &code);
        if (code < 0x80) {
            str_buf_append_char((char)code);
        } else if (code < 0x800) {
            str_buf_append_char((char)(0xC0 | (code >> 6)));
            str_buf_append_char((char)(0x80 | (code & 0x3F)));
        } else {
            str_buf_append_char((char)(0xE0 | (code >> 12)));
            str_buf_append_char((char)(0x80 | ((code >> 6) & 0x3F)));
            str_buf_append_char((char)(0x80 | (code & 0x3F)));
        }
    }

    /* Invalid escape sequence */
    \\(.|\n)            {
        fprintf(stderr, "Error: invalid escape sequence '\\%c' at line %d, column %d\n",
                yytext[1], yylineno, yycolumn - yyleng);
        json_parse_error = 1;
        return -1;
    }

    /* Unterminated string (EOF inside string) */
    <<EOF>>             {
        fprintf(stderr, "Error: unterminated string at line %d\n", yylineno);
        json_parse_error = 1;
        return -1;
    }

    /* Regular characters */
    [^\\\"\n]+          { str_buf_append(yytext, yyleng); }

    /* Newline inside string (not allowed in JSON) */
    \n                  {
        fprintf(stderr, "Error: unterminated string (newline) at line %d, column %d\n",
                yylineno, yycolumn);
        json_parse_error = 1;
        yycolumn = 1;
        return -1;
    }
}

 /* ---- Catch-all for invalid characters --------------------------- */
.                       {
    fprintf(stderr, "Error: unexpected character '%s' at line %d, column %d\n",
            yytext, yylineno, yycolumn - yyleng);
    json_parse_error = 1;
    return -1;
}

%%
