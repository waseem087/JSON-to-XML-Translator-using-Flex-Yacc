/*
 * parser.y — Bison parser for JSON → AST
 * CS-4031 Compiler Construction — Assignment 04
 * Author: Naveed Ahmed (i220889)
 *
 * Parses valid JSON input according to the ECMA-404 grammar and
 * constructs an Abstract Syntax Tree.  On success the AST root is
 * stored in `ast_root` for the XML generator to traverse.
 */

%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

/* Provided by Flex */
extern int  yylex(void);
extern int  yylineno;
extern int  yycolumn;
extern char *yytext;
extern FILE *yyin;

/* AST root — set on successful parse */
ASTNode *ast_root = NULL;

/* Error flag */
int json_parse_error = 0;

/* Bison error handler */
void yyerror(const char *msg) {
    fprintf(stderr, "Error: %s at line %d, column %d\n",
            msg, yylineno, yycolumn);
    json_parse_error = 1;
}
%}

/* ---- Value type for the parser stack ----------------------------- */
%union {
    char    *string_val;
    double   number_val;
    int      bool_val;
    ASTNode *node;
}

/* ---- Token declarations ------------------------------------------ */
%token LBRACE RBRACE LBRACKET RBRACKET COLON COMMA
%token <string_val> STRING_LIT
%token <number_val> NUMBER
%token <bool_val>   T_TRUE T_FALSE
%token T_NULL

/* ---- Non-terminal types ------------------------------------------ */
%type <node> json value object array members pair elements

/* ---- Start symbol ------------------------------------------------ */
%start json

%%

json
    : value                     { ast_root = $1; $$ = $1; }
    ;

value
    : object                    { $$ = $1; }
    | array                     { $$ = $1; }
    | STRING_LIT                { $$ = ast_new_string($1); free($1); }
    | NUMBER                    { $$ = ast_new_number($1); }
    | T_TRUE                    { $$ = ast_new_bool(1); }
    | T_FALSE                   { $$ = ast_new_bool(0); }
    | T_NULL                    { $$ = ast_new_null(); }
    ;

object
    : LBRACE RBRACE             { $$ = ast_new_object(); }
    | LBRACE members RBRACE     { $$ = $2; }
    ;

members
    : pair                      {
        $$ = ast_new_object();
        ast_add_child($$, $1);
    }
    | members COMMA pair        {
        ast_add_child($1, $3);
        $$ = $1;
    }
    ;

pair
    : STRING_LIT COLON value    { $$ = ast_new_pair($1, $3); free($1); }
    ;

array
    : LBRACKET RBRACKET         { $$ = ast_new_array(); }
    | LBRACKET elements RBRACKET { $$ = $2; }
    ;

elements
    : value                     {
        $$ = ast_new_array();
        ast_add_child($$, $1);
    }
    | elements COMMA value      {
        ast_add_child($1, $3);
        $$ = $1;
    }
    ;

%%
