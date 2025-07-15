///author: CHARLES HENRY HUTSON IV

#include "stack.h"
#include <stdio.h>
#include <stdlib.h>

/// Creates a new stack
///
/// @return A pointer to the newly created stack
stack_t *make_stack(void) {
	stack_t *stack = (stack_t *) malloc(sizeof(stack_t));
	if (stack == NULL) { // Check if malloc failed
		fprintf(stderr, "Error: Stack memory allocation failed.\n");
		exit(EXIT_FAILURE); // Use exit instead of return
	}

	stack->top = NULL; // Top pointer must be NULL per assignment
	return stack;
}

/// Pushes an element onto the stack
///
/// @param stack The stack to push the element onto
/// @param data The data to push onto the stack
void push(stack_t *stack, void *data) {
	stack_node_t *node = (stack_node_t *) malloc(sizeof(stack_node_t));
	if (node == NULL) { // Check if malloc failed
                fprintf(stderr, "Error: Stack push failed.\n");
                exit(EXIT_FAILURE);
	}

	// Make and link the node we just made
	//printf("$ %s\n", (char *) data);
	node->data = data;
	node->next = stack->top;
	stack->top = node;
}

/// Returns the top element of the stack
///
/// @param stack The stack to get the top element from
/// @return The top element of the stack
void *top(stack_t *stack) {
	if (stack->top == NULL) { // Check if stack is empty. Must check "stack->top", NOT "stack"
		fprintf(stderr, "Error: Stack is empty.\n");
		exit(EXIT_FAILURE);
	}
	//printf("$ %s\n", (char *) stack->top->data);
	return stack->top->data; // Return the top element ("stack->top->data")
}

/// Pops the top element off the stack
///
/// @param stack The stack to pop the top element from
void pop(stack_t *stack) { // Similar to above, remember to free. Do not return anything ("void", not "void*")
	if (stack->top == NULL) {
                fprintf(stderr, "Error: Stack is empty.\n");
                exit(EXIT_FAILURE);
        }

      	stack_node_t *temp = stack->top; // Save the top element before freeing
	stack->top = stack->top->next;

	// Free!
	//free(temp->data); // IF ISSUE OCCURS LOOK HERE, MAY NOT NEED TO FREE HERE
	free(temp);
}

/// Checks if the stack is empty
///
/// @param stack The stack to check
/// @return 1 if the stack is empty, 0 otherwise
int empty_stack(stack_t * stack) {
	if (stack->top == NULL) {
		// Stack is empty, so return int != 0
		return 1;
	}

	// Stack is not empty, so return 0 per assignment
	return 0;
}

/// Frees the memory allocated for the stack
///
/// @param stack The stack to free
void free_stack(stack_t * stack) { // Iterate through. Related to pop
	while (stack->top != NULL) {
		stack_node_t *temp = stack->top;
		stack->top = stack->top->next;
		free(temp->data); 
		free(temp);
	}
	free(stack);
}

