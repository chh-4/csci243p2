///author: CHARLES HENRY HUTSON IV

#define _DEFAULT_SOURCE

#include "tree_node.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/// Creates an interior node for the parse tree
///
/// @param op The operator type
/// @param token The token string
/// @param left The left child node
/// @param right The right child node
/// @return A pointer to the newly created interior node, or NULL if an error occurs
tree_node_t *make_interior(op_type_t op, char *token, tree_node_t *left, tree_node_t *right) {
    	tree_node_t *node = (tree_node_t *) malloc(sizeof(tree_node_t));
    	if (node == NULL) { // Check if node malloc failed
        	fprintf(stderr, "Error: Node memory allocation failed.\n");
		free(node);
		return NULL;
    	}

    	node->type = INTERIOR;
    	node->token = strdup(token);
    	if (node->token == NULL) { // Check if strdup failed
        	fprintf(stderr, "Error: String duplication failed.\n");
        	free(node);
        	return NULL;
    	}

    	interior_node_t *interior = (interior_node_t *) malloc(sizeof(interior_node_t));
    	if (interior == NULL) { // Check if interior malloc failed
        	fprintf(stderr, "Error: Interior node memory allocation failed.\n");
        	free(node->token);
        	free(node);

        	return NULL;
    	}

    	interior->op = op;
    	interior->left = left;
    	interior->right = right;
    	node->node = interior;

    	return node;
}

/// Creates a leaf node for the parse tree
///
/// @param exp_type The expression type
/// @param token The token string
/// @return A pointer to the newly created leaf node, or NULL if an error occurs
tree_node_t *make_leaf(exp_type_t exp_type, char *token) {
    	tree_node_t *node = (tree_node_t *) malloc(sizeof(tree_node_t));
    	if (node == NULL) { // Check if malloc failed
        	fprintf(stderr, "Error: Node memory allocation failed.\n");
		free(node->token);
		free(node);
        	return NULL;
    	}

    	node->type = LEAF;
    	node->token = strdup(token);
    	if (node->token == NULL) { // Check if strdup failed
        	fprintf(stderr, "Error: String duplication failed.\n");
		free(node);
        	return NULL;
    	}

    	leaf_node_t *leaf = (leaf_node_t *) malloc(sizeof(leaf_node_t));
    	if (leaf == NULL) { // Check if mnalloc failed
        	fprintf(stderr, "Error: Leaf node memory allocation failed.\n");
        	free(node->token);
        	free(node);

        	return NULL;
    	}

    	leaf->exp_type = exp_type;
    	node->node = leaf;

    	return node;
}
