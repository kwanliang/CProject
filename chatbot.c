/*
 * ICT1002 (C Language) Group Project, AY19 Trimester 1.
 *
 * This file implements the behaviour of the chatbot. The main entry point to
 * this module is the chatbot_main() function, which identifies the intent
 * using the chatbot_is_*() functions then invokes the matching chatbot_do_*()
 * function to carry out the intent.
 *
 * chatbot_main() and chatbot_do_*() have the same method signature, which
 * works as described here.
 *
 * Input parameters:
 *   inc      - the number of words in the question
 *   inv      - an array of pointers to each word in the question
 *   response - a buffer to receive the response
 *   n        - the size of the response buffer
 *
 * The first word indicates the intent. If the intent is not recognised, the
 * chatbot should respond with "I do not understand [intent]." or similar, and
 * ignore the rest of the input.
 *
 * If the second word may be a part of speech that makes sense for the intent.
 *    - for WHAT, WHERE and WHO, it may be "is" or "are".
 *    - for SAVE, it may be "as" or "to".
 *    - for LOAD, it may be "from".
 * The word is otherwise ignored and may be omitted.
 *
 * The remainder of the input (including the second word, if it is not one of the
 * above) is the entity.
 *
 * The chatbot's answer should be stored in the output buffer, and be no longer
 * than n characters long (you can use snprintf() to do this). The contents of
 * this buffer will be printed by the main loop.
 *
 * The behaviour of the other functions is described individually in a comment
 * immediately before the function declaration.
 *
 * You can rename the chatbot and the user by changing chatbot_botname() and
 * chatbot_username(), respectively. The main loop will print the strings
 * returned by these functions at the start of each line.
 */
 
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "chat1002.h"
 
 
/*
 * Get the name of the chatbot.
 *
 * Returns: the name of the chatbot as a null-terminated string
 */
const char *chatbot_botname() {

	return "Chatbot";
	
}


/*
 * Get the name of the user.
 *
 * Returns: the name of the user as a null-terminated string
 */
const char *chatbot_username() {

	return "User";
	
}


/*
 * Get a response to user input.
 *
 * See the comment at the top of the file for a description of how this
 * function is used.
 *
 * Returns:
 *   0, if the chatbot should continue chatting
 *   1, if the chatbot should stop (i.e. it detected the EXIT intent)
 */
int chatbot_main(int inc, char *inv[], char *response, int n) {
	
	/* check for empty input */
	if (inc < 1) {
		snprintf(response, n, "");
		return 0;
	}

	/* look for an intent and invoke the corresponding do_* function */
	if (chatbot_is_exit(inv[0]))
		return chatbot_do_exit(inc, inv, response, n);
	else if (chatbot_is_smalltalk(inv[0]))
		return chatbot_do_smalltalk(inc, inv, response, n);
	else if (chatbot_is_load(inv[0]))
		return chatbot_do_load(inc, inv, response, n);
	else if (chatbot_is_question(inv[0]))
		return chatbot_do_question(inc, inv, response, n);
	else if (chatbot_is_reset(inv[0]))
		return chatbot_do_reset(inc, inv, response, n);
	else if (chatbot_is_save(inv[0]))
		return chatbot_do_save(inc, inv, response, n);
	else {
		snprintf(response, n, "I don't understand \"%s\".", inv[0]);
		return 0;
	}
}


/*
 * Determine whether an intent is EXIT.
 *
 * Input:
 *  intent - the intent
 *
 * Returns:
 *  1, if the intent is "exit" or "quit"
 *  0, otherwise
 */
int chatbot_is_exit(const char *intent) {
	
	return compare_token(intent, "exit") == 0 || compare_token(intent, "quit") == 0;
	
}


/*
 * Perform the EXIT intent.
 *
 * See the comment at the top of the file for a description of how this
 * function is used.
 *
 * Returns:
 *   0 (the chatbot always continues chatting after a question)
 */
int chatbot_do_exit(int inc, char *inv[], char *response, int n) {
	knowledge_reset(1);
	snprintf(response, n, "Goodbye!");
	 
	return 1;
}


/*
 * Determine whether an intent is LOAD.
 *
 * Input:
 *  intent - the intent
 *
 * Returns:
 *  1, if the intent is "load"
 *  0, otherwise
 */
int chatbot_is_load(const char *intent) {
	
	return compare_token(intent, "load") == 0;
	
}


/*
 * Load a chatbot's knowledge base from a file.
 *
 * See the comment at the top of the file for a description of how this
 * function is used.
 *
 * Returns:
 *   0 (the chatbot always continues chatting after loading knowledge)
 */
int chatbot_do_load(int inc, char *inv[], char *response, int n) {

	char* fileName = get_entity(inc, inv);
	FILE *f = fopen(fileName, "r");

	// Unable to open file
	if (f == NULL)
	{
		snprintf(response, n, "UNABLE TO OPEN FILE");
		return 0;
	}
	// Reset the current Knowledge Base
	knowledge_reset(0);
	knowledge_read(f);
	fclose(f);
	free(fileName);
	snprintf(response, n, "FILE LOADED");
	return 0;
	 
}

/*
 * Determine whether an intent is a question.
 *
 * Input:
 *  intent - the intent
 *
 * Returns:
 *  1, if the intent is "what", "where", or "who"
 *  0, otherwise
 */
int chatbot_is_question(const char *intent) {
	
	return compare_token(intent, "what") == 0 || compare_token(intent, "where") == 0 || compare_token(intent, "who") == 0;
	
}


/*
 * Answer a question.
 *
 * inv[0] contains the the question word.
 * inv[1] may contain "is" or "are"; if so, it is skipped.
 * The remainder of the words form the entity.
 *
 * See the comment at the top of the file for a description of how this
 * function is used.
 *
 * Returns:
 *   0 (the chatbot always continues chatting after a question)
 */
int chatbot_do_question(int inc, char *inv[], char *response, int n) {
	
	char* entity = get_entity(inc, inv);

	// If entity not found in knowledge base
	if (knowledge_get(inv[0], entity, response, n) == KB_NOTFOUND) {

		//prompt_user
		char input[MAX_RESPONSE];
		char* tempAsk[MAX_RESPONSE];
		strcpy(tempAsk, "I don't know,");
		for (int i = 0; i < inc; ++i) {
			strcat(tempAsk, " ");
			strcat(tempAsk, inv[i]);
		}
		strcat(tempAsk, "?");
		prompt_user(input, n, tempAsk);

		if (knowledge_put(inv[0], entity, input) == KB_OK) {
			snprintf(response, n, "OK I'LL REMEMBER THIS!");
		}
		else {
			snprintf(response, n, "OH NO, SOMETHING WENT WRONG!");
		}
	}
	free(entity);
	return 0;
}


/*
 * Determine whether an intent is RESET.
 *
 * Input:
 *  intent - the intent
 *
 * Returns:
 *  1, if the intent is "reset"
 *  0, otherwise
 */
int chatbot_is_reset(const char *intent) {
	
	return compare_token(intent, "reset") == 0;
	
}


/*
 * Reset the chatbot.
 *
 * See the comment at the top of the file for a description of how this
 * function is used.
 *
 * Returns:
 *   0 (the chatbot always continues chatting after beign reset)
 */
int chatbot_do_reset(int inc, char *inv[], char *response, int n) {
	knowledge_reset(0);
	snprintf(response, n, "RESET SUCCESSFUL");
	return 0;
	 
}


/*
 * Determine whether an intent is SAVE.
 *
 * Input:
 *  intent - the intent
 *
 * Returns:
 *  1, if the intent is "save"
 *  0, otherwise
 */
int chatbot_is_save(const char *intent) {
	
	return compare_token(intent, "save") == 0;
	
}


/*
 * Save the chatbot's knowledge to a file.
 *
 * See the comment at the top of the file for a description of how this
 * function is used.
 *
 * Returns:
 *   0 (the chatbot always continues chatting after saving knowledge)
 */
int chatbot_do_save(int inc, char *inv[], char *response, int n) {
	
	char* fileName = get_entity(inc, inv);

	for (int i = 0; i < KB_SIZE; ++i) {
		if (strlen(WhatKB[i]->entity) != 0) {
			knowledge_write(fileName, "what", WhatKB[i]->entity, WhatKB[i]->response);
			if (WhatKB[i]->next != NULL) {
				KNOWLEDGE_PTR curr = WhatKB[i]->next;
				do {
					knowledge_write(fileName, "what", curr->entity, curr->response);
					curr = curr->next;
				} while (curr != NULL);
			}
		}
		if (strlen(WhoKB[i]->entity) != 0) {
			knowledge_write(fileName, "what", WhoKB[i]->entity, WhoKB[i]->response);
			if (WhoKB[i]->next != NULL) {
				KNOWLEDGE_PTR curr = WhoKB[i]->next;
				do {
					knowledge_write(fileName, "what", curr->entity, curr->response);
					curr = curr->next;
				} while (curr != NULL);
			}
		}
		if (strlen(WhereKB[i]->entity) != 0) {
			knowledge_write(fileName, "what", WhereKB[i]->entity, WhereKB[i]->response);
			if (WhereKB[i]->next != NULL) {
				KNOWLEDGE_PTR curr = WhereKB[i]->next;
				do {
					knowledge_write(fileName, "what", curr->entity, curr->response);
					curr = curr->next;
				} while (curr != NULL);
			}
		}
	}

	snprintf(response, n, "SAVE SUCCESSFUL");
	return 0;
	 
}
 
 
/*
 * Determine which an intent is smalltalk.
 *
 *
 * Input:
 *  intent - the intent
 *
 * Returns:
 *  1, if the intent is the first word of one of the smalltalk phrases
 *  0, otherwise
 */
int chatbot_is_smalltalk(const char *intent) {
	
	return compare_token(intent, "hi") == 0
		|| compare_token(intent, "hello") == 0
		|| compare_token(intent, "halo") == 0
		|| compare_token(intent, "nice") == 0
		|| compare_token(intent, "ok") == 0
		|| compare_token(intent, "I") == 0
		|| compare_token(intent, "so") == 0;

	return 0;
}


/*
 * Respond to smalltalk.
 *
 * See the comment at the top of the file for a description of how this
 * function is used.
 *
 * Returns:
 *   0, if the chatbot should continue chatting
 *   1, if the chatbot should stop chatting (e.g. the smalltalk was "goodbye" etc.)
 */
int chatbot_do_smalltalk(int inc, char *inv[], char *response, int n) {
	
	if (compare_token("Hi", inv[0]) == 0)
		snprintf(response, n, "Hi there human");

	// I need to build a bot, can you help me with that"
	else if (compare_token("I", inv[0]) == 0)
		snprintf(response, n, "Sure, more bot friends for me :)");

	// so why arent you helping me?
	else if (compare_token("So", inv[0]) == 0)
		snprintf(response, n, "you only asked if i can...");

	else if (compare_token("ok", inv[0]) == 0)
		snprintf(response, n, "hahahaha");

	//nice joke
	else if (compare_token("nice", inv[0]) == 0)
		snprintf(response, n, "Thanks, i know i am funny~ hahaha");


	return 0;
}


/*
 * Extracts the entity from the user's input.
 * 
 * If the second word may be a part of speech that makes sense for the intent.
 *    - for WHAT, WHERE and WHO, it may be "is" or "are".
 *    - for SAVE, it may be "as" or "to".
 *    - for LOAD, it may be "from".
 * The word is otherwise ignored and may be omitted.
 * 
 * Input:
 * 	inc - word count in input
 * 	inv - array of pointers to each word from user's input
 * 
 * Returns:
 * 	A pointer to a char containing the entity
 * 	If the intent is a question,
 * 		entity will contain all words after the intent or the second word
 * 	If the intent is SAVE | LOAD,
 * 		entity will contain the first word after the intent or the second word
 * 	
 * 
 * E.g. "what is this thing" 	-->	this thing
 * 		"save as that" 			-->	that
 * 		"save as that thing"	--> that
 */
char *get_entity(int inc, char *inv[])
{
	// Number of words between start of entity and start of input
	int offset = 1;
	char *intent = inv[0];
	char *entity = (char*)malloc(MAX_ENTITY);
	/* memory not allocated handling */
	if (entity == NULL) {
		printf("Memory not allocated.\n");
		exit(0);
	}
	memset(entity, '\0', sizeof(entity));

	// Intent is WHAT | WHERE | WHO
	if (chatbot_is_question(intent))
	{
		// If second word in input is IS | ARE
		if (strcmp(inv[1], "is") == 0 || strcmp(inv[1], "are") == 0)
		{
			++offset;
		}
		strjoin(entity, " ", inv, inc, offset);
	}

	// Intent is SAVE
	else if (chatbot_is_save(intent))
	{
		// If second word in input is AS | TO
		snprintf(entity, MAX_ENTITY, "%s", inv[1]);
		if (strcmp(inv[1], "as") == 0 || strcmp(inv[1], "to") == 0)
			snprintf(entity, MAX_ENTITY, "%s", inv[2]);
	}

	// Intent is LOAD
	else if (chatbot_is_load(intent))
	{
		// If second word in input is FROM
		snprintf(entity, MAX_ENTITY, "%s", inv[1]);
		if (strcmp(inv[1], "from") == 0)
			snprintf(entity, MAX_ENTITY, "%s", inv[2]);
	}

	return entity;
}

/*
 * Helper function for get_entity()
 * Implementation of Python's join() function
 * 
 * Input
 *  dest: receiver of the joined string
 *  joiner: containes the string to be inserted between each input string
 *  strings: array of strings to be joined
 *  numberOfStrings: array size of strings
 * 	offset: number of words between start of entity and start of input
 */
void strjoin(char *dest, const char *joiner, char *strings[], int numberOfStrings, int offset)
{
    // Max length of resultant string
    // including the terminating null char
    // Initialised to the first string
    char buffer[MAX_ENTITY];
    strcpy(buffer, strings[offset]);
    for (int i = offset + 1; i < numberOfStrings; ++i)
    {
        strcat(buffer, joiner);
        strcat(buffer, strings[i]);
    }
    snprintf(dest, MAX_ENTITY, "%s", buffer);
}