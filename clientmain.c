#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>


#include "clients.h"
#include "common.h"


#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/ioctl.h>

#define TRUE 1
#define FALSE 0
#define STDIN 0
#define BUFFER_SIZE 256
int drawScreen(int Onex, int Oney, int Twox, int Twoy){
  int w,h;
  struct winsize win;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &win);
  h = win.ws_row;
  w = win.ws_col;
  for(int i = 0; i < h; i++){ //LOOP THROUGH ROWS
    if(i == 0 | i == h-1){//TOP | BOTTOM
      for(int j = 0; j < w; j++){
	putchar('*');
      }
    }else{
      putchar('*');
      for(int k = 0; k < w-2; k++){
        if(k == Onex && i == Oney){
          putchar('X');
        }else if (k == Twox && i == Twoy){
          putchar('X');
        }else{
          putchar(' ');
        }
      }
      putchar('*');
    }
  }
  return 0;
}


int main(int argc, char *argv[]){
    int playerX = 40;
    int playerY = 13;
    int otherX = 40;
    int otherY = 13;

    if(argc != 3){   
      printf("Wrong number of arguments!\n Expected: [LISTENING_PORT] and [SERVER_PORT]\n");
      return -1;
    }else{
      //LISTENING PORT NUMBER
      int port = atoi(argv[1]);

      //IP ADDRESS
      char *ipAddr = (char *)malloc(INET_ADDRSTRLEN);
      get_pip(ipAddr);
      printf("Starting Client on Port: %d\n", port);

      //SET UP SELECT SYSTEM CALL NONSENSE

      int server_sock, max_sock, selret, sock_index, client_sock = 0, caddr_len;
      struct sockaddr_in client;
      struct addrinfo hints, *res;

      socklen_t client_len;

      fd_set master_list, watch_list;
      char buffer[BUFFER_SIZE];

      memset(&hints, 0, sizeof(hints));

      hints.ai_family = AF_INET;
      hints.ai_socktype = SOCK_STREAM;
      hints.ai_flags = AI_PASSIVE;

      if(getaddrinfo(ipAddr, argv[1], &hints, &res) != 0){
        perror("getaddrinfo failed :(\n");
      }
      server_sock = socket (res->ai_family, res->ai_socktype, res->ai_protocol);

      if(bind(server_sock, res->ai_addr, res->ai_addrlen) < 0){
        perror("Server bind fail\n");
        exit(EXIT_FAILURE);
      }

      freeaddrinfo(res);

      if(listen(server_sock, 4) < 0){
        perror("socket listen failure\n");
        close(server_sock);
        exit(EXIT_FAILURE);
      }


      FD_ZERO(&master_list);
      FD_ZERO(&watch_list);

      FD_SET(server_sock, &master_list);
      FD_SET(STDIN, &master_list);

      max_sock = server_sock;


      //CONNECT TO SERVER
      int serversock;
      //MAKE SOCKET
      struct sockaddr_in newSock;
      int serverPort = atoi(argv[2]);
      if ((serversock = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        perror("Error getting socket FD\n");
        return 0;
      }

      memset(&newSock, 0, sizeof(newSock));
      newSock.sin_family = AF_INET;
      newSock.sin_addr.s_addr = inet_addr(ipAddr);
      // CHANGE PORT TO INT
      newSock.sin_port = htons(serverPort);


      if (connect(serversock, (struct sockaddr *)&newSock, sizeof(newSock)) != 0){
        perror("Error connecting to external server\n");
        close(serversock);
        return 0;
      }


      //THE WHILE LOOP BEINGS!!!
      while(TRUE){
        // SAVING CAUSE IDK
        drawScreen(playerX, playerY, otherX, otherY);

        memcpy(&watch_list, &master_list, sizeof(master_list));
 
        fflush(stdout);

        selret = select(client_sock + 1, &watch_list, NULL, NULL, NULL);

        if (selret < 0)
          perror("select failed\n");

        if (selret > 0)
        {
          for (sock_index = 0; sock_index <= client_sock; sock_index++)
          {
            if (FD_ISSET(sock_index, &watch_list)){
              if (sock_index == STDIN){
                char *msg = get_command_str();
                for(int i = 0; i < strlen(msg); i++){
                  if(msg[i] == 'w'){
                    playerY--;
                  }else if(msg[i] == 's'){
                    playerY++;
                  }else if(msg[i] == 'a'){
                    playerX--;
                  }else if(msg[i] == 'd'){
                    playerX++;
                  }
                }
                //MAKE NEW X AND Y COORDINATES, PACKAGE, AND SEND

                char x[2] = {'\0'};
                char y[2]= {'\0'};
                sprintf(x, "%d", playerX);
                sprintf(y, "%d", playerY);


                if(send(serversock, msg, strlen(msg), 0) < 0){
                  printf("Failed to send %s\n", msg);
                }
              }
              else{
                char *buffer = (char *)malloc(sizeof(char) * BUFFER_SIZE);
                memset(buffer, '\0', BUFFER_SIZE);

                printf("ARE WE EVEN GETTING HERE????  1\n");

                if (recv(sock_index, buffer, BUFFER_SIZE, 0) <= 0)
                {
                  close(sock_index);
                  printf("Disconnected from Server.\n");

                  FD_CLR(sock_index, &master_list);
                }
                else
                {
                  printf("ARE WE EVEN GETTING HERE????  2\n");
                  printf("RECIEVING:%s", buffer);
                  
                }
                memset(buffer, '\0', BUFFER_SIZE);
                fflush(stdout);
              }
            }
          }
        }
      }

      //

      return 0;
  }
}
