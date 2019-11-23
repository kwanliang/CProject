#pragma once

/* return codes for knowledge_get() and knowledge_put() */
#define KB_OK        0
#define KB_NOTFOUND -1
#define KB_INVALID  -2
#define KB_NOMEM    -3

/* the maximum number of Knowledge allowed in a cache */
#define KB_SIZE 67

/* for caching of response */
typedef struct Knowledge {
	char* entity;
	char* response;
	struct Knowledge* next;
}KNOWLEDGE;
typedef KNOWLEDGE* KNOWLEDGE_PTR;

/* Knowledge Base for intent */
KNOWLEDGE_PTR WhatKB[KB_SIZE];
KNOWLEDGE_PTR WhoKB[KB_SIZE];
KNOWLEDGE_PTR WhereKB[KB_SIZE];

/* functions for knowledge base management */
KNOWLEDGE_PTR CreateKnowledge(char* entity, char* response);
KNOWLEDGE_PTR BlankKnowledge();
KNOWLEDGE_PTR GetEndKnowledge(KNOWLEDGE_PTR head);
char* GetResponse(KNOWLEDGE_PTR head, char* entity);
int hash_entity(const char* entity);
void ResetLinkedList(KNOWLEDGE_PTR head);
void FreeLinkedList(KNOWLEDGE_PTR head);
void ResetNode(KNOWLEDGE_PTR node);

/* functions defined in knowledge.c */
void knowledge_init();
int knowledge_get(const char* intent, const char* entity, char* response, int n);
int knowledge_put(const char* intent, const char* entity, const char* response);
void knowledge_reset(int exit);
void knowledge_read(FILE* f);
int knowledge_write(char* pacPath, char* pacSection, char* pacKey, char* pacValue);