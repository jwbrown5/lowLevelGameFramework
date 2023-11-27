
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>


#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "common.h"

void get_pip(char *thisIP){
  int sockfd;
  struct sockaddr_in dummySock;

  if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
    printf("ERROR GETTING SOCKET FILE DESCRIPTOR");
  }

  memset(&dummySock, 0, sizeof(dummySock));
  dummySock.sin_family = AF_INET;
  dummySock.sin_addr.s_addr = inet_addr("8.8.8.8");
  dummySock.sin_port = htons(53);

  //THE CODE ABOVE REACHES OUT TO GOOGLES DNS SERVER TO FORCE THE KERNEL TO RETURN THE IP ADDRESS OF THIS SOCKET.

  if(connect(sockfd, (struct sockaddr *)&dummySock, sizeof(dummySock)) < 0){
    perror("ERROR RETURNING SOCK");
    close(sockfd);
  }

  struct sockaddr_in thisAddr;
  socklen_t thisIplen = sizeof(thisAddr);

  getsockname(sockfd, (struct sockaddr *)&thisAddr, &thisIplen);

  inet_ntop(AF_INET, &thisAddr.sin_addr, thisIP, INET_ADDRSTRLEN);


  struct in_addr tempAddr;

  inet_pton(AF_INET, thisIP, &tempAddr);
}


char *get_command_str()
{
    char *msg = (char*) malloc(sizeof(char)*256);
    memset(msg, '\0', 256);

    if(fgets(msg, 256-1, stdin) == NULL) //Mind the newline character that will be written to msg
		exit(-1);
    //Remove trailing newline
    msg[strlen(msg) - 1] = '\0';

    return msg;
}