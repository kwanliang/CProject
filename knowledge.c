/*
 * ICT1002 (C Language) Group Project, AY19 Trimester 1.
 *
 * This file implements the chatbot's knowledge base.
 *
 * knowledge_get() retrieves the response to a question.
 * knowledge_put() inserts a new response to a question.
 * knowledge_read() reads the knowledge base from a file.
 * knowledge_reset() erases all of the knowledge.
 * kowledge_write() saves the knowledge base in a file.
 *
 * You may add helper functions as necessary.
 */
 
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "knowledge.h"
#include "chat1002.h"

void knowledge_init() {
	for (int i = 0; i < KB_SIZE; ++i) {
		WhatKB[i] = BlankKnowledge();
		WhoKB[i] = BlankKnowledge();
		WhereKB[i] = BlankKnowledge();
	}
}

/*
 * Get the response to a question.
 *
 * Input:
 *   intent   - the question word
 *   entity   - the entity
 *   response - a buffer to receive the response
 *   n        - the maximum number of characters to write to the response buffer
 *
 * Returns:
 *   KB_OK, if a response was found for the intent and entity (the response is copied to the response buffer)
 *   KB_NOTFOUND, if no response could be found
 *   KB_INVALID, if 'intent' is not a recognised question word
 */
int knowledge_get(const char *intent, const char *entity, char *response, int n) {
	if (compare_token(intent, "what") == 0) {
		int hashIndex = hash_entity(entity);
		char* tempResponse = GetResponse(WhatKB[hashIndex], entity);	
		if (tempResponse != NULL) {
			snprintf(response, n, tempResponse);
			return KB_OK;
		} else return KB_NOTFOUND;
	} else if (compare_token(intent, "who") == 0) {
		int hashIndex = hash_entity(entity);
		char* tempResponse = GetResponse(WhoKB[hashIndex], entity);
		if (tempResponse != NULL) {
			snprintf(response, n, tempResponse);
			return KB_OK;
		} else return KB_NOTFOUND;
	} else if (compare_token(intent, "where") == 0) {
		int hashIndex = hash_entity(entity);
		char* tempResponse = GetResponse(WhereKB[hashIndex], entity);
		if (tempResponse != NULL) {
			snprintf(response, n, tempResponse);
			return KB_OK;
		} else return KB_NOTFOUND;
	}
	
	return KB_INVALID;
}


/*
 * Insert a new response to a question. If a response already exists for the
 * given intent and entity, it will be overwritten. Otherwise, it will be added
 * to the knowledge base.
 *
 * Input:
 *   intent    - the question word
 *   entity    - the entity
 *   response  - the response for this question and entity
 *
 * Returns:
 *   KB_OK, if successful
 *   KB_NOMEM, if there was a memory allocation failure
 *   KB_INVALID, if the intent is not a valid question word
 */
int knowledge_put(const char *intent, const char *entity, const char *response) {

	if (compare_token(intent, "what") == 0) {
		// Calculate a index from the entity
		int hashIndex = hash_entity(entity);
		// Set Knowledge if empty
		if (WhatKB[hashIndex]->entity == "") {
			WhatKB[hashIndex]->entity = entity;
			WhatKB[hashIndex]->response = response;
		}
		// Otherwise, append to the end its linked list
		else 
			GetEndKnowledge(WhatKB[hashIndex])->next = CreateKnowledge(entity, response);
	}
	else if (compare_token(intent, "who") == 0) {
		// Calculate a index from the entity
		int hashIndex = hash_entity(entity);
		// Set Knowledge if empty
		if (WhoKB[hashIndex]->entity == "") {
			WhoKB[hashIndex]->entity = entity;
			WhoKB[hashIndex]->response = response;
		}
		// Otherwise, append to the end its linked list
		else
			GetEndKnowledge(WhoKB[hashIndex])->next = CreateKnowledge(entity, response);
	}
	else if (compare_token(intent, "where") == 0) {
		// Calculate a index from the entity
		int hashIndex = hash_entity(entity);
		// Set Knowledge if empty
		if (WhereKB[hashIndex]->entity == "") {
			WhereKB[hashIndex]->entity = entity;
			WhereKB[hashIndex]->response = response;
		}
		// Otherwise, append to the end its linked list
		else
			GetEndKnowledge(WhereKB[hashIndex])->next = CreateKnowledge(entity, response);
	}

	return KB_OK;
	
}


/*
 * Read a knowledge base from a file.
 *
 * Input:
 *   f - the file
 *
 * Returns: the number of entity/response pairs successful read from the file
 */
int knowledge_read(FILE *f) {
	FILE* fp = fopen("sample.ini", "r"); //opens sample.ini located at this file's location
	if (fp == NULL) {
		perror("Unable to open file!");
		exit(1);
	}

	char chunk[128];

	size_t len = sizeof(chunk);
	char* line = malloc(len);
	if (line == NULL) {
		perror("Unable to allocate memory for the line buffer.");
		exit(1);
	}

	line[0] = '\0';

	while (fgets(chunk, sizeof(chunk), fp) != NULL) {
		size_t len_used = strlen(line);
		size_t chunk_used = strlen(chunk);

		if (len - len_used < chunk_used) {
			len *= 2;
			if ((line = realloc(line, len)) == NULL) {
				perror("Unable to reallocate memory for the line buffer.");
				free(line);
				exit(1);
			}
		}

		strncpy(line + len_used, chunk, len - len_used);
		len_used += chunk_used;

		if (line[len_used - 1] == '\n') {
			fputs(line, stdout);
			//fputs("|*\n", stdout);
			line[0] = '\0';
		}
	}

	fclose(fp);
	free(line);
	
	return 0;
}


/*
 * Reset the knowledge base, removing all know entitities from all intents.
 */
void knowledge_reset() {
	
	
}


/*
 * Write the knowledge base to a file.
 *
 * Input:
 *   f - the file
 */
void knowledge_write(FILE *f) {
	
	char sentence[1000]; //variable for testing purposes
	FILE* fptr;

	fptr = fopen("sample.txt", "w"); //creates a sample.txt at this file's location
	if (fptr == NULL) {
		printf("Error!");
		exit(1);
	}

	fgets(sentence, 1000, stdin); //user's input for the knowledge 
	fprintf(fptr, "%s", sentence); //writes input into sample.txt
	fclose(fptr); //closes and saves the file

	
}


/*
 * Create a new knowledge and assign given entity and response
 *
 * Input:
 *   entity    - the entity
 *   response  - the response for this question and entity
 *
 * Returns:
 *   KNOWLEDGE_PTR, the created knowledge
 */
KNOWLEDGE_PTR CreateKnowledge(char* entity, char* response) {
	KNOWLEDGE_PTR tempKnowledge = (KNOWLEDGE_PTR)malloc(sizeof(KNOWLEDGE_PTR));
	tempKnowledge->entity = entity;
	tempKnowledge->response = response;
	tempKnowledge->next = NULL;
	return tempKnowledge;
}

/*
 * Create a new knowledge and initialize the variables
 *
 * Input:
 *   entity    - the entity
 *   response  - the response for this question and entity
 *
 * Returns:
 *   KNOWLEDGE_PTR, the created knowledge
 */
KNOWLEDGE_PTR BlankKnowledge() {
	KNOWLEDGE_PTR tempKnowledge = (KNOWLEDGE_PTR)malloc(sizeof(KNOWLEDGE_PTR));
	tempKnowledge->entity = "";
	tempKnowledge->response = "";
	tempKnowledge->next = NULL;
	return tempKnowledge;
}

/*
 * Search for response in a Knowledge Base with matching entity 
 *
 * Input:
 *   head	   - the start of a linked list of a Knowledge Base
 *   entity    - the entity to search for
 *
 * Returns:
 *   char*, the response if found
 *	 NULL, otherwise
 */
char* GetResponse(KNOWLEDGE_PTR head, char* entity) {
	KNOWLEDGE_PTR curr = head;
	do {
		if (compare_token(curr->entity, entity) == 0)
			return curr->response;
		curr = curr->next;
	} while (curr != NULL);

	return NULL;
}

/*
 * Get the Knowledge at the end of a linked list of a Knowledge Base
 *
 * Input:
 *   head	   - the start of a linked list of a Knowledge Base
 *
 * Returns:
 *   KNOWLEDGE_PTR, the pointer to the end Knowledge
 */
KNOWLEDGE_PTR GetEndKnowledge(KNOWLEDGE_PTR head) {
	KNOWLEDGE_PTR curr = head;
	while (curr->next != NULL) {
		curr = curr->next;
	}
	return curr;
}

/*
 * Hashes string into integer to be used as index for Knowledge Base
 *
 * Input:
 *   entity	   - the entity
 *
 * Returns:
 *   int, the index after hashing
 */
int hash_entity(const char* entity) {
	int total = 0; // Total value count of each character
	int it = 0; // While loop iterator
	while (entity[it] != '\0') {
		total += (int)entity[it] + it + 1; // Add decimal of char and iterator index to total
		it++; // Increment While loop iterator
	}
	return total % KB_SIZE; // Return hashed index that fits within range of KB_SIZE 
}