/*
 * main.c — Entry point for json2xml
 * CS-4031 Compiler Construction — Assignment 04
 * Author: Naveed Ahmed (i220889)
 *
 * Reads JSON from stdin, parses it, optionally prints the AST,
 * and generates the corresponding XML to stdout.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "xmlgen.h"

/* Provided by the parser */
extern int   yyparse(void);
extern FILE *yyin;
extern ASTNode *ast_root;
extern int json_parse_error;

int main(int argc, char *argv[]) {
    int show_ast = 0;

    /* Check for optional --ast flag */
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--ast") == 0) {
            show_ast = 1;
        } else {
            fprintf(stderr, "Usage: json2xml [--ast] < input.json\n");
            return 1;
        }
    }

    /* Parse from stdin */
    yyin = stdin;

    int parse_result = yyparse();

    if (parse_result != 0 || json_parse_error) {
        /* Error message already printed by yyerror / lexer */
        if (ast_root) ast_free(ast_root);
        return 1;
    }

    if (!ast_root) {
        fprintf(stderr, "Error: empty input\n");
        return 1;
    }

    /* Bonus: print AST if requested */
    if (show_ast) {
        printf("=== Abstract Syntax Tree ===\n");
        ast_print(ast_root, 0);
        printf("============================\n\n");
    }

    /* Generate XML */
    xml_generate(ast_root);

    /* Clean up */
    ast_free(ast_root);

    return 0;
}
