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

 /* Return pointer to first char (of chars) or inline comment in given string,
	or pointer to null at end of string if neither found. Inline comment must
	be prefixed by a whitespace character to register as a comment. */
static char* find_chars_or_comment(const char* s, const char* chars)
{
#if INI_ALLOW_INLINE_COMMENTS
	int was_space = 0;
	while (*s && (!chars || !strchr(chars, *s)) &&
		!(was_space && strchr(INI_INLINE_COMMENT_PREFIXES, *s))) {
		was_space = isspace((unsigned char)(*s));
		s++;
	}
#else
	while (*s && (!chars || !strchr(chars, *s))) {
		s++;
	}
#endif
	return (char*)s;
}


/* Return pointer to first non-whitespace char in given string. */
static char* lskip(const char* s)
{
	while (*s && isspace((unsigned char)(*s)))
		s++;
	return (char*)s;
}

/* Strip whitespace chars off end of given string, in place. Return s. */
static char* rstrip(char* s)
{
	char* p = s + strlen(s);
	while (p > s&& isspace((unsigned char)(*--p)))
		*p = '\0';
	return s;
}

/* Version of strncpy that ensures dest (size bytes) is null-terminated. */
static char* strncpy0(char* dest, const char* src, size_t size)
{
	strncpy(dest, src, size - 1);
	dest[size - 1] = '\0';
	return dest;
}

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
int knowledge_get(const char* intent, const char* entity, char* response, int n) {
	if (compare_token(intent, "what") == 0) {
		int hashIndex = hash_entity(entity);
		char* tempResponse = GetResponse(WhatKB[hashIndex], entity);
		if (tempResponse != NULL) {
			snprintf(response, n, tempResponse);
			return KB_OK;
		}
		else return KB_NOTFOUND;
	}
	else if (compare_token(intent, "who") == 0) {
		int hashIndex = hash_entity(entity);
		char* tempResponse = GetResponse(WhoKB[hashIndex], entity);
		if (tempResponse != NULL) {
			snprintf(response, n, tempResponse);
			return KB_OK;
		}
		else return KB_NOTFOUND;
	}
	else if (compare_token(intent, "where") == 0) {
		int hashIndex = hash_entity(entity);
		char* tempResponse = GetResponse(WhereKB[hashIndex], entity);
		if (tempResponse != NULL) {
			snprintf(response, n, tempResponse);
			return KB_OK;
		}
		else return KB_NOTFOUND;
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
int knowledge_put(const char* intent, const char* entity, const char* response) {

	if (compare_token(intent, "what") == 0) {
		// Calculate a index from the entity
		int hashIndex = hash_entity(entity);
		// Set Knowledge if empty
		if (strlen(WhatKB[hashIndex]->entity) == 0) {
			strcpy(WhatKB[hashIndex]->entity, entity);
			strcpy(WhatKB[hashIndex]->response, response);
		}
		// Otherwise, append to the end its linked list
		else
			GetEndKnowledge(WhatKB[hashIndex])->next = CreateKnowledge(entity, response);
	}
	else if (compare_token(intent, "who") == 0) {
		// Calculate a index from the entity
		int hashIndex = hash_entity(entity);
		// Set Knowledge if empty
		if (strlen(WhoKB[hashIndex]->entity) == 0) {
			strcpy(WhoKB[hashIndex]->entity, entity);
			strcpy(WhoKB[hashIndex]->response, response);
		}
		// Otherwise, append to the end its linked list
		else
			GetEndKnowledge(WhoKB[hashIndex])->next = CreateKnowledge(entity, response);
	}
	else if (compare_token(intent, "where") == 0) {
		// Calculate a index from the entity
		int hashIndex = hash_entity(entity);
		// Set Knowledge if empty
		if (strlen(WhereKB[hashIndex]->entity) == 0) {
			strcpy(WhereKB[hashIndex]->entity, entity);
			strcpy(WhereKB[hashIndex]->response, response);
		}
		// Otherwise, append to the end its linked list
		else
			GetEndKnowledge(WhereKB[hashIndex])->next = CreateKnowledge(entity, response);
	}

	return KB_OK;
}


/*
 * Re-initialize a KNOWLEDGE node.
 *
 * Input:
 *   KNOWLEDGE_PTR, linked list node
 */
void ResetNode(KNOWLEDGE_PTR node) {
	strcpy(node->entity, "");
	strcpy(node->response, "");
	node->next = NULL;
}

/*
 * Reset the knowledge base, removing all known entities from all intents.
 *
 * Input:
 *   KNOWLEDGE_PTR, head of linked list which is each index of Knowledge Base
 */
void ResetLinkedList(KNOWLEDGE_PTR head) {
	KNOWLEDGE_PTR curr = head;
	KNOWLEDGE_PTR next = curr->next;

	// if only head exist
	if (next == NULL) {
		// re-initialize head and return
		ResetNode(curr);
		return;
	}
	else {
		// keep memory of head, only reinitialize head
		ResetNode(curr);
		// move on to the next node
		curr = next;
		next = next->next;
	}

	// free memory of subsequent nodes
	do {
		free(curr);
		curr = next;
		next = next->next;
	} while (curr != NULL);
}

/*
 * Free up all allocated memory to a Knowledge Base
 *
 * Input:
 *   KNOWLEDGE_PTR, head of linked list which is each index of Knowledge Base
 */
void FreeLinkedList(KNOWLEDGE_PTR head) {
	KNOWLEDGE_PTR curr = head;
	KNOWLEDGE_PTR next = curr->next;

	// if only head exist
	if (next == NULL) {
		// free head memory
		free(curr);
		return;
	}

	// free memory of subsequent nodes
	do {
		free(curr);
		curr = next;
		next = next->next;
	} while (curr != NULL);
}


/*
 * Reset the knowledge base, removing all known entities from all intents.
 *
 * Input:
 *   int, 1 if exiting the program, 0 otherwise
 */
void knowledge_reset(int exit) {
	if (exit) {
		// free all allocation memory
		for (int i = 0; i < KB_SIZE; ++i) {
			FreeLinkedList(WhatKB[i]);
			FreeLinkedList(WhoKB[i]);
			FreeLinkedList(WhereKB[i]);
		}
	}
	else {
		// free all allocation memory, except head node
		for (int i = 0; i < KB_SIZE; ++i) {
			ResetLinkedList(WhatKB[i]);
			ResetLinkedList(WhoKB[i]);
			ResetLinkedList(WhereKB[i]);
		}
	}
}


/*
 * Read a knowledge base from a file.
 *
 * Input:
 *   f - the file
 *
 * Returns: the number of entity/response pairs successful read from the file
 */
void knowledge_read(FILE* f) {
	char section[MAX_SECTION] = "";
	char line[128]; /* or other suitable maximum line size */
	while (fgets(line, sizeof(line), f) != NULL) /* read a line */
	{
		char* start;
		char* end;
		char* name;
		char* value;

		//printf("%s", line);
		start = line;
		start = lskip(rstrip(start));

		if (*start == '[') {
			end = find_chars_or_comment(start + 1, "]");
			if (*end == ']') {
				*end = '\0';
				strncpy0(section, start + 1, sizeof(section));
				// printf("\n\n%s \n", section);
			}
		}
		else if (*start) {
			end = find_chars_or_comment(start, "=");
			if (*end == '=') {
				*end = '\0';
				name = rstrip(start);
				value = end + 1;
				value = lskip(value);
				rstrip(value);
				knowledge_put(section, name, value);
			}
		}
	}
}

/*
 * Write the knowledge base to a file.
 *
 * Input:
 *   knowledge_write(<filename>, <section>, <key>, <value>);
 */
int knowledge_write(char* pacPath, char* pacSection, char* pacKey, char* pacValue) {
	//module variables
	int iFoundSection;
	int iFoundKey;
	int iError;
	long SectionFilePos;
	int iKeyLength;
	int iValueLength;
	char acSectionHeading[80];
	char acLastSectionHeading[80];
	char acKeyHeading[80];
	char acIniLine[160];
	char acIniPath[160];
	char acTempPath[160];
	FILE* pFIniFile;
	FILE* pFTempIni;

	iError = 0;
	acLastSectionHeading[0] = '\0';

	strcpy(acIniPath, pacPath);

	strcpy(acTempPath, pacPath);
	strcat(acTempPath, "temp");

	// add brackets to section
	strcpy(acSectionHeading, "[");
	strcat(acSectionHeading, pacSection);
	strcat(acSectionHeading, "]\n");
	// adds '=' to key
	strcpy(acKeyHeading, pacKey);
	strcat(acKeyHeading, "=");

	iKeyLength = strlen(acKeyHeading);
	iValueLength = strlen(pacValue);

	iFoundSection = 0;
	iFoundKey = 0;

	if ((pFTempIni = fopen(acTempPath, "w")) == NULL) {
		printf("Could not open temp ini file to write settings\n");
		iError = 1;
		return (iError);
	}

	// try to open current config file for reading
	if ((pFIniFile = fopen(acIniPath, "r")) != NULL) {
		// read a line from the config file until EOF
		while (fgets(acIniLine, 159, pFIniFile) != NULL) {
			// the key has been found so continue reading file to end
			if (iFoundKey == 1) {
				fputs(acIniLine, pFTempIni);
				continue;
			}
			// section has not been found yet
			if (iFoundSection == 0) {
				if (strcmp(acSectionHeading, acIniLine) == 0) {
					// found the section
					iFoundSection = 1;
				}
				fputs(acIniLine, pFTempIni);
				continue;
			}
			// the key has not been found yet
			if ((iFoundKey == 0) && (iFoundSection == 1)) {
				if (strncmp(acKeyHeading, acIniLine, iKeyLength) == 0) {
					// found the key
					iFoundKey = 1;
					if (iValueLength > 0) {
						fputs(acKeyHeading, pFTempIni);
						fputs(pacValue, pFTempIni);
						fputs("\n", pFTempIni);
					}
					continue;
				}
				// if newline or [, the end of the section has been reached
				// so add the key to the section
				if ((acIniLine[0] == '\n') || (acIniLine[0] == '[')) {
					iFoundKey = 1;
					if (iValueLength > 0) {
						fputs(acKeyHeading, pFTempIni);
						fputs(pacValue, pFTempIni);
						fputs("\n", pFTempIni);
					}
					fputs("\n", pFTempIni);
					if (acIniLine[0] == '[') {
						fputs(acIniLine, pFTempIni);
					}
					continue;
				}
				// if the key has not been found, write line to temp file
				if (iFoundKey == 0) {
					fputs(acIniLine, pFTempIni);
					continue;
				}
			}
		}
		fclose(pFIniFile);
	}
	// still did not find the key after reading the file
	if (iFoundKey == 0) {
		// file does not exist
		// or section does not exist so write to temp file
		if (iValueLength > 0) {
			if (iFoundSection == 0) {
				fputs(acSectionHeading, pFTempIni);
			}
			fputs(acKeyHeading, pFTempIni);
			fputs(pacValue, pFTempIni);
			fputs("\n\n", pFTempIni);
		}
	}

	fclose(pFTempIni);

	//delete the ini file
	remove(acIniPath);

	// rename the temp file to ini file
	rename(acTempPath, acIniPath);

	return (iError);
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
	KNOWLEDGE_PTR tempKnowledge = (KNOWLEDGE_PTR)malloc(sizeof(KNOWLEDGE));

	//check whether memory allocation was successful
	//if successful, return the pointer
	if (tempKnowledge != NULL) {
		strcpy(tempKnowledge->entity, entity);
		strcpy(tempKnowledge->response, response);
		tempKnowledge->next = NULL;
		return tempKnowledge;
	}
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
	KNOWLEDGE_PTR tempKnowledge = (KNOWLEDGE_PTR)malloc(sizeof(KNOWLEDGE));

	//check whether memory allocation was successful
	//if successful, return the pointer
	if (tempKnowledge != NULL) {
		strcpy(tempKnowledge->entity, "");
		strcpy(tempKnowledge->response, "");
		tempKnowledge->next = NULL;
		return tempKnowledge;
	}

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

	// For case-insensitive matching
	char* entity_lower = (char*)malloc(MAX_ENTITY);

	//check whether memory allocation was successful
	//if successful, return the pointer
	if (entity_lower != NULL) {
		strncpy(entity_lower, entity, MAX_ENTITY);
		for (char* it = entity_lower; (*it) != '\0'; ++it)
		{
			(*it) = tolower(*it);
		}

		while (entity_lower[it] != '\0') {
			total += (int)entity_lower[it] + it + 1; // Add decimal of char and iterator index to total
			it++; // Increment While loop iterator
		}
		free(entity_lower);
		return total % KB_SIZE; // Return hashed index that fits within range of KB_SIZE 
	}

}