//author: CHARLES HENRY HUTSON IV

#define _DEFAULT_SOURCE // This caused a headache. VERY IMPORTANT
#include "symtab.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

static symbol_t *head = NULL;

/// Builds the symbol table from the given file
///
/// @param filename The name of the file containing the symbol table
void build_table(char *filename) { // Per assignment, variable names are unique. Static or dynamic local storage
	// Format: variable-type variable-name     variable-value
	FILE *file = fopen(filename, "r");
	if (file == NULL) { // Check if file open failed
		perror(filename);
		exit(EXIT_FAILURE);
	}

	char buffer[BUFLEN];
	while (fgets(buffer, sizeof(buffer), file)) {
		// Skip the lines starting with '#'
		if (buffer[0] == '#') {
			continue;
		}

		char var_name[BUFLEN];
		int var_value;
		if (sscanf(buffer, "%s %d", var_name, &var_value) == 2) { // This is so much better than the way I was originally going to do this...
			// A symbol is an alphanumeric string starting with an alphabetic character
			if(isalpha(var_name[0])) {
				create_symbol(var_name, var_value);
				//printf("%s", var_name);
				//printf("%d", var_value);
			} else {
				// Enforce correct symbol name
				fprintf(stderr, "Error: Invalid symbol name.\n");
				fclose(file);
				exit(EXIT_FAILURE);
			}
		} else {
			// If sscanf() does not return 2 then the line did NOT match the correct format
			fprintf(stderr, "Error: Symbol table line contains incorrect format.\n");
			fclose(file);
			exit(EXIT_FAILURE);
		}
	}

	// Everything occurred correctly, so just close the file
	fclose(file);
}

/// Dumps the contents of the symbol table
void dump_table(void) { // Print the contents of the symbol table
	printf("\nSYMBOL TABLE:\n");
	symbol_t *current = head;
	while (current != NULL) {
		printf("\tName: %s, Value: %d\n", current->var_name, current->val);
		current = current->next;
	}
}

/// Looks up a symbol in the table
///
/// @param variable The name of the variable to look up
/// @return The symbol if found, NULL otherwise
symbol_t  *lookup_table(char * variable) {
	symbol_t *current = head;
	while (current != NULL) {
		if (strcmp(current->var_name, variable) == 0) { // Then we found the variable with the name we wanted, so return the symbol object
			return current;
		}
		current = current->next;
	}

	return NULL; // We did not find the variable with the name, so return NULL per assignment
}

/// Creates a new symbol and adds it to the table
///
/// @param name The name of the symbol
/// @param val The value of the symbol
/// @return The newly created symbol
symbol_t *create_symbol(char *name, int val) {
	symbol_t *new_symbol = (symbol_t *) malloc(sizeof(symbol_t));
	if (new_symbol == NULL) { // Check if malloc failed
		fprintf(stderr,"Error: symbol memory allocation failed.\n");
		exit(EXIT_FAILURE);
	}

	new_symbol->var_name = strdup(name);
	if (new_symbol->var_name == NULL) { // Check if malloc failed
		fprintf(stderr,"Error: symbol name memory allocation failed.\n");
		free(new_symbol);
		exit(EXIT_FAILURE);
	}

	new_symbol->val = val;
	new_symbol->next = head;
	head = new_symbol;

	return new_symbol;
}

/// Frees the memory allocated for the symbol table
void free_table(void) {
	symbol_t *current = head;
	while (current != NULL) { // Iterate through the table
		symbol_t *next = current->next;
		free(current->var_name);
		free(current);
		current = next;
	}

	//current = NULL;
	head = NULL;
}
