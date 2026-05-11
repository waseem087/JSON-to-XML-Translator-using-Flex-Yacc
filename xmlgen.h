/*
 * xmlgen.h — XML generation from AST
 * CS-4031 Compiler Construction — Assignment 04
 * Author: Naveed Ahmed (i220889)
 *
 * Traverses the AST produced by the parser and emits well-formed XML
 * to stdout with proper indentation (bonus: pretty-printed XML).
 */

#ifndef XMLGEN_H
#define XMLGEN_H

#include "ast.h"
#include <stdio.h>
#include <string.h>
#include <math.h>

/* ------------------------------------------------------------------ */
/*  Helper: print 'depth' levels of indentation                        */
/* ------------------------------------------------------------------ */
static inline void xml_indent(int depth) {
    for (int i = 0; i < depth; i++)
        printf("  ");
}

/* ------------------------------------------------------------------ */
/*  Helper: escape XML-special characters in text content              */
/*  Handles &, <, >, ", and '                                         */
/* ------------------------------------------------------------------ */
static inline void xml_escape_print(const char *s) {
    for (; *s; s++) {
        switch (*s) {
            case '&':  printf("&amp;");   break;
            case '<':  printf("&lt;");    break;
            case '>':  printf("&gt;");    break;
            case '"':  printf("&quot;");  break;
            case '\'': printf("&apos;");  break;
            default:   putchar(*s);       break;
        }
    }
}

/* ------------------------------------------------------------------ */
/*  Forward declaration                                                */
/* ------------------------------------------------------------------ */
static void xml_emit(ASTNode *node, const char *tag, int depth);

/* ------------------------------------------------------------------ */
/*  Emit a value node wrapped in <tag>...</tag>                        */
/*  'tag' may be NULL for the root call (wrapper added by caller).     */
/* ------------------------------------------------------------------ */
static void xml_emit(ASTNode *node, const char *tag, int depth) {
    if (!node) return;

    switch (node->type) {

        /* --- Object: emit each key-value pair as a child element --- */
        case NODE_OBJECT:
            if (tag) {
                xml_indent(depth);
                printf("<%s>\n", tag);
            }
            for (int i = 0; i < node->child_count; i++) {
                ASTNode *pair = node->children[i];
                /* pair->string_val is the key, pair->children[0] is the value */
                xml_emit(pair->children[0], pair->string_val, depth + (tag ? 1 : 0));
            }
            if (tag) {
                xml_indent(depth);
                printf("</%s>\n", tag);
            }
            break;

        /* --- Array: wrap each element in <item> -------------------- */
        case NODE_ARRAY:
            if (tag) {
                xml_indent(depth);
                printf("<%s>\n", tag);
            }
            for (int i = 0; i < node->child_count; i++) {
                xml_emit(node->children[i], "item", depth + (tag ? 1 : 0));
            }
            if (tag) {
                xml_indent(depth);
                printf("</%s>\n", tag);
            }
            break;

        /* --- String ------------------------------------------------ */
        case NODE_STRING:
            xml_indent(depth);
            printf("<%s>", tag);
            xml_escape_print(node->string_val);
            printf("</%s>\n", tag);
            break;

        /* --- Number ------------------------------------------------ */
        case NODE_NUMBER: {
            xml_indent(depth);
            printf("<%s>", tag);
            /* Print integers without decimal point */
            if (node->number_val == floor(node->number_val) &&
                fabs(node->number_val) < 1e15) {
                printf("%.0f", node->number_val);
            } else {
                printf("%g", node->number_val);
            }
            printf("</%s>\n", tag);
            break;
        }

        /* --- Boolean ----------------------------------------------- */
        case NODE_BOOL:
            xml_indent(depth);
            printf("<%s>%s</%s>\n", tag,
                   node->bool_val ? "true" : "false", tag);
            break;

        /* --- Null → self-closing tag ------------------------------- */
        case NODE_NULL:
            xml_indent(depth);
            printf("<%s/>\n", tag);
            break;

        /* NODE_PAIR should not appear here directly */
        case NODE_PAIR:
            break;
    }
}

/* ------------------------------------------------------------------ */
/*  Public entry point: generate XML for the whole AST                 */
/* ------------------------------------------------------------------ */
static inline void xml_generate(ASTNode *root) {
    printf("<root>\n");
    /* The root of a JSON document is always an object or array.
       For an object, emit its pairs directly inside <root>. 
       For an array, emit its elements as <item> inside <root>. */
    if (root) {
        if (root->type == NODE_OBJECT) {
            for (int i = 0; i < root->child_count; i++) {
                ASTNode *pair = root->children[i];
                xml_emit(pair->children[0], pair->string_val, 1);
            }
        } else if (root->type == NODE_ARRAY) {
            for (int i = 0; i < root->child_count; i++) {
                xml_emit(root->children[i], "item", 1);
            }
        } else {
            /* single scalar at top level (unusual but valid JSON) */
            xml_emit(root, "value", 1);
        }
    }
    printf("</root>\n");
}

#endif /* XMLGEN_H */
