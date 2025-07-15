/*
 * interp.c
 *
 * author: CHARLES HENRY HUTSON IV
 * date: NOV 24 2024
 */

#define _DEFAULT_SOURCE

#include "interp.h"
#include "symtab.h"
#include "stack.h"
#include "tree_node.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

static int error_flag = 0; // Global error flag

/// Evaluates the given parse tree node
///
/// @param node The parse tree node to evaluate
/// @return The evaluated integer value, or -1 if an error occurs
int eval(tree_node_t *node) {
	if (error_flag) {
        	return -1; // Terminate if an error has occurred
    	}

    	if (node->type == LEAF) {
        	leaf_node_t *leaf = (leaf_node_t *) node->node;
        	if (leaf->exp_type == INTEGER) {
            		return strtol(node->token, NULL, 10);

        	} else if (leaf->exp_type == SYMBOL) {
            		symbol_t *symbol = lookup_table(node->token);

            		if (symbol == NULL) {
                		fprintf(stderr, "Error: Undefined symbol.\n");
                		error_flag = 1; // Set error flag
                		return -1;
            		}
			return symbol->val;
        	}

    	} else if (node->type == INTERIOR) {
        	interior_node_t *interior = (interior_node_t *) node->node;

        	if (interior->op == ASSIGN_OP) {
            		if (interior->left->type != LEAF || ((leaf_node_t *) interior->left->node)->exp_type != SYMBOL) {
                		fprintf(stderr, "Error: Invalid l-value.\n");
                		error_flag = 1; // Set error flag
                		return -1;
            		}
            		int val = eval(interior->right);

			if (error_flag) {
                		return -1; // Propagate error
            		}

            		symbol_t *symbol = lookup_table(interior->left->token);
            		if (symbol == NULL) {
                		symbol = create_symbol(interior->left->token, val);
            		} else {
                		symbol->val = val;
            		}

            		return val;

        } else if (interior->op == Q_OP) {
            	int test_val = eval(interior->left);
		if (error_flag) {
                	return -1; // Propagate error
            	}
            	interior_node_t *alt_node = (interior_node_t *) interior->right->node;

		if (test_val) {
                	return eval(alt_node->left);

            	} else {
                	return eval(alt_node->right);
            	}
        } else {
            	int left_val = eval(interior->left);

		if (error_flag) {
                	return -1; // Propagate error
            	}

            	int right_val = eval(interior->right);

		if (error_flag) {
                	return -1; // Propagate error
            	}

            		switch (interior->op) { // Shoutout SI session for going over these
                		case ADD_OP: return left_val + right_val;
                		case SUB_OP: return left_val - right_val;
                		case MUL_OP: return left_val * right_val;
                		case DIV_OP:
					if (right_val == 0) {
                        			fprintf(stderr, "Error: Division by zero.\n");
                        			error_flag = 1; // Set error flag
                        			return -1;
                    			}
                    			return left_val / right_val;
                		case MOD_OP:
					if (right_val == 0) {
                        			fprintf(stderr, "Error: Division by zero.\n");
                        			error_flag = 1; // Set error flag
                        			return -1;
                    			}
                    			return left_val % right_val;
                		default: fprintf(stderr, "Error: Unknown operation.\n");
				error_flag = 1; // Set error flag
				return -1;
            		}
        	}
    	}
    	fprintf(stderr, "Error: Unkonwn node type.\n");
	error_flag = 1; // Set error flag
    	return -1;
}

/// Prints the parse tree with the infix notation - "Easiest part of the assignment"
///
/// @param node The parse tree node to print
void print_infix(tree_node_t *node) {
    	if (node->type == LEAF) {
        	printf("%s", node->token);

    	} else if (node->type == INTERIOR) {
        	interior_node_t *interior = (interior_node_t *)node->node;

        	printf("(");

        	print_infix(interior->left);
        	printf("%s", node->token);
        	print_infix(interior->right);

        	printf(")");
    	}
}

/// Checks if the given token is an operator
///
/// @param token The token to check
/// @return 1 if the token is an operator, 0 otherwise
int is_op(char *token) { // Check if the token is an operation per tree_node.h
	return (strcmp(token, ADD_OP_STR) == 0 || strcmp(token, SUB_OP_STR) == 0 ||
           	strcmp(token, MUL_OP_STR) == 0 || strcmp(token, DIV_OP_STR) == 0 ||
           	strcmp(token, MOD_OP_STR) == 0 || strcmp(token, ASSIGN_OP_STR) == 0 ||
           	strcmp(token, Q_OP_STR) == 0); // There may be an easier way to do this but KISS right?
}

/// Gets the operator type for the given token
///
/// @param token The token to get the operator type for
/// @return The operator type, or NO_OP if the token is not a valid operator
op_type_t get_op_type(char *token) {
	if (strcmp(token, ADD_OP_STR) == 0) {
		return ADD_OP;
	} else if (strcmp(token, SUB_OP_STR) == 0) {
		return SUB_OP;
	} else if (strcmp(token, MUL_OP_STR) == 0) {
		return MUL_OP;
	} else if (strcmp(token, DIV_OP_STR) == 0) {
		return DIV_OP;
	} else if (strcmp(token, MOD_OP_STR) == 0) {
		return MOD_OP;
    	} else if (strcmp(token, ASSIGN_OP_STR) == 0) {
		return ASSIGN_OP;
    	} else if (strcmp(token, Q_OP_STR) == 0) {
		return Q_OP;
    	} else {
		fprintf(stderr, "Error: Unknown operation.\n");
		error_flag = 1; // Set error flag
    		return NO_OP;
	} // In retrospect I think I could've used a switch case and done it better :(
}

/// Checks if the given token is an integer
///
/// @param token The token to check
/// @return 1 if the token is an integer, 0 otherwise
int is_int(char *token) { // Check if this token is an int. Only needs to check for positive numbers. Return 0 if not an int, 1 if the token is an int
	if (isdigit(token[0])) { // Make sure first digit is a number
		 for (int i = 1; token[i] != '\0'; i++) { // Iterate through and confirm each one is a number
			if (isdigit(token[i])) {
				continue;
			} else {
				return 0; // Found a non-number
			}
		}
	} else {
		return 0; // Found a non-number
	}
	// It must be a number, so return 1
	return 1;
}

/// Checks if the given token is a symbol
///
/// @param token The token to check
/// @return 1 if the token is a symbol, 0 otherwise
int is_symbol(char *token) { // Very similar to above, but for symbols
	if (isalpha(token[0])) { // Make sure first digit is a letter
		 for (int i = 1; token[i] != '\0'; i++) { // Iterate through and confirm each one is a letter or number
			if (isalnum(token[i])) {
				continue;
			} else {
				return 0; // Found non-letter or non-number
			}
		}
	} else {
		return 0; // Found a non-letter or non-number
	}
	// It must be a symbol, so return 1
	return 1;
}

/// Parses the given stack into a parse tree
///
/// @param stack The stack to parse
/// @return The root of the parse tree, or NULL if an error occurs
tree_node_t *parse(stack_t *stack) { // Build the parse tree from the stack made in the tokenize function
    	if (empty_stack(stack)) { // Check if stack is empty
        	fprintf(stderr, "Error: Stack is empty.\n");
        	exit(EXIT_FAILURE); // This one is a fatal error
    	}

    	char *token = (char *) top(stack);

    	pop(stack);
	// Figure out what kind of token (a+10)>we are dealing with and handle them appropriately
    	if (is_op(token)) {
        	op_type_t op = get_op_type(token);
        	if (op == NO_OP) return NULL; // Non-fatal error
        	if (op == Q_OP) {
            		tree_node_t *false_expr = parse(stack);
            		if (false_expr == NULL) return NULL; // Propagate error
            		tree_node_t *true_expr = parse(stack);
            		if (true_expr == NULL) return NULL; // Propagate error
            		tree_node_t *test_expr = parse(stack);
            		if (test_expr == NULL) return NULL; // Propagate error
            		tree_node_t *alt_node = make_interior(ALT_OP, ":", true_expr, false_expr);

            		return make_interior(Q_OP, token, test_expr, alt_node);
        	} else {
            		tree_node_t *right = parse(stack);
            		if (right == NULL) return NULL; // Propagate error
            		tree_node_t *left = parse(stack);
            		if (left == NULL) return NULL; // Propagate error

            		return make_interior(op, token, left, right);
        	}
    	} else if (is_int(token)) {
        	return make_leaf(INTEGER, token);
    	} else if (is_symbol(token)) {
        	return make_leaf(SYMBOL, token);
    	} else {
        	fprintf(stderr, "Error: Illegal token.\n");
        	error_flag = 1; // Set error flag
        	return NULL;
	// I don't think I like parserland
    	}
}

void eval_and_print(char line[]) { // Handles tokenizations and some simple valuations that there is no reason to delay on
	// Tokenize
	stack_t *tokens = make_stack();
	int num_tokens = 0;

	char *token = strtok(line, " ");
	while (token != NULL) {
		num_tokens++;
		push(tokens, token);
		//printf("%s\n", token);
		token = strtok(NULL, " ");
	}

	if (num_tokens > 0) {
        	tree_node_t *root = parse(tokens);
        	if (root == NULL || error_flag) {
            		error_flag = 0; // Reset error flag
            		printf("> ");
            		return;
        	}

        	print_infix(root);
        	int result = eval(root);
        	if (!error_flag) {
            		printf(" = %d\n", result);
        	}
    	} // Always need the >
    	printf("> ");
}

/// The main function of the interpreter program
///
/// @param argc The number of command line arguments
/// @param argv The command line arguments
/// @return EXIT_SUCCESS if no error, or EXIT_FAILURE on a (fatal)error
int main(int argc, char *argv[]) {
	if (argc > 2) {
		fprintf(stderr, "Usage: interp [sym-table]\n");
		return EXIT_FAILURE; // Fatal error

	} else if (argc == 2) {
		//printf("Building table.\n");
		build_table(argv[1]);
		//printf("Dumping table.\n");
		dump_table();

		printf("Enter postfix expressions (CTRL-D to exit):\n");
		// Read-eval-print loop
		char buffer[BUFLEN];
		printf("> ");
		while (fgets(buffer, sizeof(buffer), stdin)) { // Stdin takes any input from user
			// Ignore lines starting with '#'
			if (buffer[0] == '#') {
				printf("> ");
				continue;
			}

			// If line has a '#', but not at beginning, ignore everything after it
			// Assignment receommends usage of "strchr" and replacing '#' with a null byte... clever
			char * ignore = strchr(buffer, '#'); // Search the buffer for the first #
			if (ignore) { // If there is a # and ignore is initialized
				*ignore = '\0';
			}

			// Perform similar action as above to trim off newlines
			char * newline = strchr(buffer, '\n');
			if (newline) {
				*newline = '\0';
			}

			// Read of line complete, send it over to the eval
			//printf("\"%s\"\n", buffer);
			eval_and_print(buffer);
			}
		} else if (argc == 1) {
			printf("Enter postfix expressions (CTRL-D to exit):\n");

                	char buffer[BUFLEN];
                	printf("> ");
                	while (fgets(buffer, sizeof(buffer), stdin)) { // Stdin takes any input from user
                        	// Ignore lines starting with '#'
                        	if (buffer[0] == '#') {
                                	printf("> ");
                                	continue;
                        	}

                        	// If line has a '#', but not at beginning, ignore everything after it
                        	// Assignment receommends usage of "strchr" and replacing '#' with a null byte... clever
                        	char * ignore = strchr(buffer, '#'); // Search the buffer for the first #
                        	if (ignore) { // If there is a # and ignore is initialized
                                	*ignore = '\0';
                        	}

                        	// Perform similar action as above to trim off newlines
                        	char * newline = strchr(buffer, '\n');
                        	if (newline) {
                                	*newline = '\0';
                        	}

                        	// Read of line complete, send it over to the eval
                        	//printf("\"%s\"\n", buffer);
                        	eval_and_print(buffer);
			}
	}
	dump_table();
	return EXIT_SUCCESS;
}
