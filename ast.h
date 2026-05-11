/*
 * ast.h — Abstract Syntax Tree definitions for JSON to XML Translator
 * CS-4031 Compiler Construction — Assignment 04
 * Author: Naveed Ahmed (i220889)
 *
 * Defines node types and the ASTNode structure used to represent
 * parsed JSON as a tree. Each node carries a type tag and the
 * relevant payload (string, number, boolean, children, etc.).
 */

#ifndef AST_H
#define AST_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* ------------------------------------------------------------------ */
/*  Node‑type enumeration                                              */
/* ------------------------------------------------------------------ */
typedef enum {
    NODE_OBJECT,    /* { ... }           — children stored in 'children' */
    NODE_ARRAY,     /* [ ... ]           — children stored in 'children' */
    NODE_STRING,    /* "text"            — value in 'string_val'        */
    NODE_NUMBER,    /* 42 / 3.14 / 1e10  — value in 'number_val'        */
    NODE_BOOL,      /* true / false      — value in 'bool_val'          */
    NODE_NULL,      /* null                                              */
    NODE_PAIR       /* "key" : value     — key in 'string_val',
                                           value in children[0]         */
} NodeType;

/* ------------------------------------------------------------------ */
/*  AST node                                                           */
/* ------------------------------------------------------------------ */
typedef struct ASTNode {
    NodeType type;

    /* Payload — which field is meaningful depends on 'type' */
    char  *string_val;      /* NODE_STRING, NODE_PAIR (key)             */
    double number_val;      /* NODE_NUMBER                              */
    int    bool_val;        /* NODE_BOOL  (1 = true, 0 = false)         */

    /* Children (for NODE_OBJECT, NODE_ARRAY, NODE_PAIR) */
    struct ASTNode **children;
    int    child_count;
    int    child_capacity;
} ASTNode;

/* ------------------------------------------------------------------ */
/*  Constructor helpers                                                */
/* ------------------------------------------------------------------ */

/* Create a generic node with a given type */
static inline ASTNode *ast_new_node(NodeType type) {
    ASTNode *n = (ASTNode *)calloc(1, sizeof(ASTNode));
    if (!n) { fprintf(stderr, "Error: out of memory\n"); exit(1); }
    n->type = type;
    n->child_capacity = 4;
    n->children = (ASTNode **)calloc(n->child_capacity, sizeof(ASTNode *));
    return n;
}

/* Append a child to a parent node */
static inline void ast_add_child(ASTNode *parent, ASTNode *child) {
    if (parent->child_count >= parent->child_capacity) {
        parent->child_capacity *= 2;
        parent->children = (ASTNode **)realloc(parent->children,
                            parent->child_capacity * sizeof(ASTNode *));
    }
    parent->children[parent->child_count++] = child;
}

/* --- Leaf constructors -------------------------------------------- */

static inline ASTNode *ast_new_string(const char *s) {
    ASTNode *n = ast_new_node(NODE_STRING);
    n->string_val = strdup(s);
    return n;
}

static inline ASTNode *ast_new_number(double v) {
    ASTNode *n = ast_new_node(NODE_NUMBER);
    n->number_val = v;
    return n;
}

static inline ASTNode *ast_new_bool(int v) {
    ASTNode *n = ast_new_node(NODE_BOOL);
    n->bool_val = v;
    return n;
}

static inline ASTNode *ast_new_null(void) {
    return ast_new_node(NODE_NULL);
}

/* --- Composite constructors --------------------------------------- */

static inline ASTNode *ast_new_pair(const char *key, ASTNode *value) {
    ASTNode *n = ast_new_node(NODE_PAIR);
    n->string_val = strdup(key);
    ast_add_child(n, value);
    return n;
}

static inline ASTNode *ast_new_object(void) {
    return ast_new_node(NODE_OBJECT);
}

static inline ASTNode *ast_new_array(void) {
    return ast_new_node(NODE_ARRAY);
}

/* ------------------------------------------------------------------ */
/*  Free the entire tree                                               */
/* ------------------------------------------------------------------ */
static inline void ast_free(ASTNode *n) {
    if (!n) return;
    for (int i = 0; i < n->child_count; i++)
        ast_free(n->children[i]);
    free(n->children);
    free(n->string_val);
    free(n);
}

/* ------------------------------------------------------------------ */
/*  Pretty‑print the AST (bonus feature)                               */
/* ------------------------------------------------------------------ */
static inline void ast_print(ASTNode *n, int indent) {
    if (!n) return;
    for (int i = 0; i < indent; i++) printf("  ");

    switch (n->type) {
        case NODE_OBJECT:
            printf("OBJECT (%d pairs)\n", n->child_count);
            break;
        case NODE_ARRAY:
            printf("ARRAY (%d elements)\n", n->child_count);
            break;
        case NODE_STRING:
            printf("STRING: \"%s\"\n", n->string_val);
            break;
        case NODE_NUMBER:
            printf("NUMBER: %g\n", n->number_val);
            break;
        case NODE_BOOL:
            printf("BOOL: %s\n", n->bool_val ? "true" : "false");
            break;
        case NODE_NULL:
            printf("NULL\n");
            break;
        case NODE_PAIR:
            printf("PAIR: \"%s\" =>\n", n->string_val);
            break;
    }

    for (int i = 0; i < n->child_count; i++)
        ast_print(n->children[i], indent + 1);
}

#endif /* AST_H */
