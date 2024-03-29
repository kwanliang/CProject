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
#include <time.h>
#include <Windows.h>
#include "chat1002.h"


 /*
  * Get the name of the chatbot.
  *
  * Returns: the name of the chatbot as a null-terminated string
  */
const char* chatbot_botname() {

	return "Chatbot";

}


/*
 * Get the name of the user.
 *
 * Returns: the name of the user as a null-terminated string
 */
const char* chatbot_username() {

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
int chatbot_main(int inc, char* inv[], char* response, int n) {

	/* check for empty input */
	if (inc < 1) {
		snprintf(response, n, "");
		return 0;
	}
	
	/* look for an intent and invoke the corresponding do_* function */
	if (chatbot_is_exit(inv[0]))
		return chatbot_do_exit(inc, inv, response, n);
	else if (chatbot_do_smalltalk(inc, inv, response, n))
		return 0;
	else if (chatbot_is_load(inv[0]))
		return chatbot_do_load(inc, inv, response, n);
	else if (chatbot_is_question(inv[0]))
		return chatbot_do_question(inc, inv, response, n);
	else if (chatbot_is_reset(inv[0]))
		return chatbot_do_reset(inc, inv, response, n);
	else if (chatbot_is_save(inv[0]))
		return chatbot_do_save(inc, inv, response, n);
	else if (chatbot_is_play(inv[0]))
		return chatbot_do_play(inc, inv, response, n);
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
int chatbot_is_exit(const char* intent) {

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
int chatbot_do_exit(int inc, char* inv[], char* response, int n) {
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
int chatbot_is_load(const char* intent) {

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
int chatbot_do_load(int inc, char* inv[], char* response, int n) {

	if (inv[1] == NULL) {
		snprintf(response, n, "Please enter file name to load from.");
		return 0;
	}
	else {
		char* fileName = get_entity(inc, inv);
		FILE* f = fopen(fileName, "r");

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
int chatbot_is_question(const char* intent) {

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
int chatbot_do_question(int inc, char* inv[], char* response, int n) {

	if (inc > 1) {
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
	else {
		snprintf(response, n, "I don't understand!");
		return 0;
	}
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
int chatbot_is_reset(const char* intent) {

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
int chatbot_do_reset(int inc, char* inv[], char* response, int n) {
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
int chatbot_is_save(const char* intent) {

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
int chatbot_do_save(int inc, char* inv[], char* response, int n) {

	if (inc == 1) {
		snprintf(response, n, "I don't understand!");
		return 0;
	}

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
			knowledge_write(fileName, "who", WhoKB[i]->entity, WhoKB[i]->response);
			if (WhoKB[i]->next != NULL) {
				KNOWLEDGE_PTR curr = WhoKB[i]->next;
				do {
					knowledge_write(fileName, "who", curr->entity, curr->response);
					curr = curr->next;
				} while (curr != NULL);
			}
		}
		if (strlen(WhereKB[i]->entity) != 0) {
			knowledge_write(fileName, "where", WhereKB[i]->entity, WhereKB[i]->response);
			if (WhereKB[i]->next != NULL) {
				KNOWLEDGE_PTR curr = WhereKB[i]->next;
				do {
					knowledge_write(fileName, "where", curr->entity, curr->response);
					curr = curr->next;
				} while (curr != NULL);
			}
		}
	}

	snprintf(response, n, "SAVE SUCCESSFUL");
	return 0;
}

int chatbot_is_play(const char* intent) {

	return compare_token(intent, "play") == 0;

}

int chatbot_do_play(int inc, char* inv[], char* response, int n) {

	char choice[5];

	int R = 1;
	int P = 2;
	int S = 3;
	int i;
	int Pscore = 0;
	int Cscore = 0;
	srand(time(NULL));
	printf("\n");
	printf("*** ROCK PAPER SCISSORS ***\n");
	printf("-----best of 5 rounds-----\n");
	printf("\n");
	printf("Rock = 1\nPaper = 2\nScissors = 3\n");
	printf("\n");

	for (i = 0; i < 5; i++) {
		printf("Enter your choice: ");
		fgets(choice, 5, stdin);

		int computer = rand() % 3 + 1;

		if (choice[0] == '1') {
			if (computer == 1) {
				printf("Computer chose rock. (1)\n");
				printf("Draw!\n");
				printf("\n");
			}
			if (computer == 2) {
				printf("Computer chose paper. (2)\n");
				printf("Computer Wins!\n");
				Cscore = Cscore + 1;
				printf("\n");
			}
			if (computer == 3) {
				printf("Computer chose scissors. (3)\n");
				printf("Player Wins!\n");
				Pscore = Pscore + 1;
				printf("\n");
			}
		}
		else if (choice[0] == '2') {
			if (computer == 2) {
				printf("Computer chose paper. (2)\n");
				printf("Draw!\n");
				printf("\n");
			}
			if (computer == 3) {
				printf("Computer chose scissors. (3)\n");
				printf("Computer Wins!\n");
				Cscore = Cscore + 1;
				printf("\n");
			}
			if (computer == 1) {
				printf("Computer chose rock. (1)\n");
				printf("Player Wins!\n");
				Pscore = Pscore + 1;
				printf("\n");
			}
		}
		else if (choice[0] == '3') {
			if (computer == 3) {
				printf("Computer chose scissors. (3)\n");
				printf("Draw!\n");
				printf("\n");
			}
			if (computer == 2) {
				printf("Computer chose paper. (2)\n");
				printf("Player Wins!\n");
				Pscore = Pscore + 1;
				printf("\n");
			}
			if (computer == 1) {
				printf("Computer chose rock.(1)\n");
				printf("Computer Wins!\n");
				Cscore = Cscore + 1;
				printf("\n");
			}
		}
		else {
			printf("Wrong Answer! Please enter 1 / 2 / 3!\n");
		}

	}
	if (Cscore > Pscore) {
		snprintf(response, n, "Computer wins %d to %d!\n", Cscore, Pscore);
	}
	else if (Cscore < Pscore) {
		snprintf(response, n, "Player wins %d to %d!\n", Pscore, Cscore);
	}
	else if (Cscore = Pscore) {
		snprintf(response, n, "No winner it is a draw!\n");
	}


	return 0;
}

/*
 * Query for smalltalk from .ini file.
 *
 * Input:
 *   inc		- amount of words from user input
 *	 inv[]		- array of words from user input
 *	 response	- the response for this small talk
 *	 n			- size of response buffer
 *
 * Returns:
 *   0, if the query result failed
 *   1, otherwise
 */
int chatbot_do_smalltalk(int inc, char* inv[], char* response, int n) {
	// Combine user input into a sentence
	char temp[MAX_INPUT] = "\0";
	for (int i = 0; i < inc; ++i) {
		// Lowercase every words
		for (int j = 0; j < strlen(inv[i]); ++j)
			inv[i][j] = tolower(inv[i][j]);
		// Join words into sentence
		strcat(temp, inv[i]);
		if (i != inc - 1)
			strcat(temp, " ");
	}
	char result[255];
	// Set the directory of the file to query from
	GetCurrentDirectory(255, result);
	strcat(result, "/smalltalk.ini");
	// Query sentence from .ini file
	GetPrivateProfileString("smalltalk", temp, NULL, result, 255, result);
	// if no result is found
	if (strlen(result) == 0)
		return 0;
	// Return the response from the query
	snprintf(response, n, result);
	return 1;
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
char* get_entity(int inc, char* inv[])
{
	// Number of words between start of entity and start of input
	int offset = 1;
	char* intent = inv[0];
	char* entity = (char*)malloc(MAX_ENTITY);
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
void strjoin(char* dest, const char* joiner, char* strings[], int numberOfStrings, int offset)
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