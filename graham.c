/*
graham.c : client
INFO-F201 : Systeme d'exploitation
Projet 2 : Photo pool management software
Nom : Nikita Servais
Matricule : 000463055
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <arpa/inet.h>	//inet_addr


#define PORT 5555
#define BUFFER_SIZE 1024


void send_photo(int socket, char *argv[])
{
  int in_fd, rd_count;
  char block[BUFFER_SIZE];

  in_fd = open(argv[3], O_RDONLY); // open the input file
  // printf("%d",in_fd);
  if (in_fd < 0) exit(2); // if it cannot be opened, exit
  while(1) // copy loop
  {
    rd_count = read(in_fd, block, BUFFER_SIZE); // read a block of data
    if (rd_count <= 0) break; // if end of file or error, exit loop
    send(socket, block, BUFFER_SIZE, 0);
  }
  close(in_fd);
  // j'ai pas eu le temps de gerer les erreurs :/
  if(rd_count == 0){
    printf("Aucune erreur : 0\n");
    exit(0);
  }
  else{
    printf("Erreur : 1\n");
    exit(5);
  }
}

void send_data(int argc, char *argv[], int sockfd)
{
  send(sockfd, argv[2], BUFFER_SIZE, 0); 
  send(sockfd, argv[4], BUFFER_SIZE, 0); 
  send(sockfd, argv[5], BUFFER_SIZE, 0); 
  send(sockfd, argv[6], BUFFER_SIZE, 0); 
  send(sockfd, argv[7], BUFFER_SIZE, 0); 
  send_photo(sockfd, argv);
}


int main(int argc, char *argv[])
{
  int sockfd;
  struct sockaddr_in server;
  
  // if ((he=gethostbyname(argv[1])) == NULL) 
  // { 
  //   perror("Error: gethostbyname\n");
  //   return EXIT_FAILURE;
  // }
  
  
  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) 
  {
    perror("Error: socket\n");
    return EXIT_FAILURE;
  }
  
  server.sin_family = AF_INET;  
  server.sin_port = htons(PORT);
  server.sin_addr.s_addr = inet_addr(argv[1]); 
  
  
  if (connect(sockfd, (struct sockaddr *)&server, sizeof(struct sockaddr)) == -1) 
  {
    perror("Error: can't connect");
    return EXIT_FAILURE;
  }

  send_data(argc, argv, sockfd);
  recv(sockfd, response, 1, 0);
  char response[1];
  printf("%s",response);

  close(sockfd);
  return EXIT_SUCCESS;
} 


