#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct clientList{
    int sockFD;
    struct clientList *prev;
    struct clientList *next;
};


struct clientList *createClientEntry(int socket);

struct clientList *insertClientEntry(struct clientList *newNode);

