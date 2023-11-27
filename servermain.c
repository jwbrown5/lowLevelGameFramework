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
	if(i == Onex && k == Oney){
	  putchar('X');
	}else if (i == Twox && k == Twoy){
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

  int connected = 0;

  struct clientList* clientOne = NULL;
  struct clientList* clientTwo = NULL;


  if(argc != 2){   
    printf("Wrong number of arguments!!");
    return -1;
  }else{
    //LISTENING PORT NUMBER
    int port = atoi(argv[1]);

    //IP ADDRESS
    char *ipAddr = (char *)malloc(INET_ADDRSTRLEN);
    get_pip(ipAddr);
    printf("Starting Server on Port: %d\n", port);

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
      perror("getaddrinfo failed :(");
    }
    server_sock = socket (res->ai_family, res->ai_socktype, res->ai_protocol);

    if(bind(server_sock, res->ai_addr, res->ai_addrlen) < 0){
      perror("Server bind fail");
      exit(EXIT_FAILURE);
    }

    freeaddrinfo(res);

    if(listen(server_sock, 4) < 0){
      perror("socket listen failure");
      close(server_sock);
      exit(EXIT_FAILURE);
    }


    FD_ZERO(&master_list);
    FD_ZERO(&watch_list);

    FD_SET(server_sock, &master_list);
    FD_SET(STDIN, &master_list);

    max_sock = server_sock;

    while(TRUE){
      //MAKE COPY OF MASTER LIST
      memcpy(&watch_list, &master_list, sizeof(master_list));

      fflush(stdout);

      selret = select(max_sock+1, &watch_list, NULL, NULL, NULL);

      if(selret < 0)
	      perror("failed to select");

      if(selret > 0){
        //LOOP THROUGH FILE DESCRIPTORS THAT CHANGED O YEA
        for(sock_index = 0; sock_index <= max_sock; sock_index++){
          if(FD_ISSET(sock_index, &watch_list)){
            if(sock_index == STDIN){ // STANDARD INPUT

              printf("Parsing from standard input not yet implemented.\n");
            }else if(sock_index == server_sock){ //CONNECTING A CLIENT
              
              client_len = sizeof(client);
              client_sock = accept(server_sock, (struct sockaddr *)&client, &client_len);
              if(client_sock < 0)
                perror("Accepting new client failed\n");


              if(connected == 0){
       
                clientOne = createClientEntry(client_sock);
		            connected = 1;
              }else{
                clientTwo = createClientEntry(client_sock);
              }
              FD_SET(client_sock, &master_list);

              if (client_sock > max_sock)
                max_sock = client_sock;
              
            }else{
              char *buffer = (char *)malloc(sizeof(char) * BUFFER_SIZE);
              memset(buffer, '\0', BUFFER_SIZE);
 
              if (recv(sock_index, buffer, BUFFER_SIZE, 0) <= 0)
              {
                close(sock_index);
		            printf("%s", buffer);
                printf("The client has left the chat!\n");

                FD_CLR(sock_index, &master_list);
              }else{
                if(sock_index == clientOne->sockFD){
                  printf("cleint %d sent: %s\n",clientOne->sockFD,buffer);
                  if(send(clientTwo->sockFD, buffer, strlen(buffer),0) < 0){
                    printf("Failed to send to alternate");
                  }
                }else{
                  printf("client %d said: %s",clientTwo->sockFD,buffer);
                  if(send(clientOne->sockFD, buffer, strlen(buffer), 0) < 0){
                    printf("failed to send, sorry man :(");
                  }
                }
              }
            }
          }
	      }
      }
    } 
  }
  return 0;
}

