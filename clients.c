#include "clients.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>


struct clientList *createClientEntry(int socket){
    struct clientList *newClient = (struct clientList*)malloc(sizeof(struct clientList));
    newClient->sockFD = socket;
    newClient->next = NULL;
    newClient->prev = NULL;

    return newClient;
}


