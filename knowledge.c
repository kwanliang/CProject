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
#include "chat1002.h"

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

	for (int i = 0; i < CacheCounter; ++i) {
		if (compare_token(knowledgeCache[i].intent, intent) != 0)
			continue;
		if (compare_token(knowledgeCache[i].entity, entity) != 0)
			continue;

		response = knowledgeCache[i].response;
		return KB_OK;
	}
	
	return KB_NOTFOUND;
	
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

	if (CacheCounter > MAX_CACHE)
		return KB_NOMEM;

	struct Knowledge temp = { .intent = intent, .entity = entity, .response = response};
	knowledgeCache[CacheCounter] = temp;
	CacheCounter++;

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
int knowledge_read(FILE *fp) {
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
			fputs("|*\n", stdout);
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
	
	memset(knowledgeCache, 0, sizeof(knowledgeCache));
	
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