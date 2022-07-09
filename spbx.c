/*
spbx.c : serveur
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
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/stat.h>

#define PORT 5555
#define BACKLOG 10
#define BUFFER_SIZE 1024

void recv_photo(int socket, FILE *tmp)
{
  int rd_count, wt_count;
  char block[BUFFER_SIZE];

  // Reception of photo by buffer size of 1024 byte
  // And stored in /tmp
  if (tmp == NULL)
    exit(1);
  while (1)
  {
    rd_count = recv(socket, block, BUFFER_SIZE, 0);
    if (rd_count <= 0)
      break;

    wt_count = fwrite(block, BUFFER_SIZE, sizeof(char), tmp);
    if (wt_count <= 0)
      exit(4);
  }
  rewind(tmp);
  if (rd_count < 0)
    exit(5);
}

void creat_directory(char *path[], char username[], char year[], char month[], char day[])
{
  strcat(*path, "/");
  strcat(*path, username);
  mkdir(*path, 0755);

  strcat(*path, "/");
  strcat(*path, year);
  mkdir(*path, 0755);

  strcat(*path, "/");
  strcat(*path, month);
  mkdir(*path, 0755);

  strcat(*path, "/");
  strcat(*path, day);
  mkdir(*path, 0755);
}

void recording_photo(char path[], char photoName[], FILE *tmp)
{
  char block[BUFFER_SIZE];
  int rd_count, wt_count;
  FILE *out_fd;

  strcat(path, "/");
  strcat(path, photoName);

  out_fd = fopen(path, "w");
  while (1)
  {
    rd_count = fread(block, sizeof(char), BUFFER_SIZE, tmp);
    if (rd_count <= 0)
      break;
    wt_count = fwrite(block, BUFFER_SIZE, sizeof(char), out_fd);
    if (wt_count <= 0)
      exit(5);
  }
}

void recv_protocol(int socket, char path[])
{
  char username[BUFFER_SIZE];
  char year[BUFFER_SIZE];
  char month[BUFFER_SIZE];
  char day[BUFFER_SIZE];
  char photoName[BUFFER_SIZE];
  FILE *tmp;
  tmp = tmpfile();

  // Reception of username, date and name of the photo
  recv(socket, username, BUFFER_SIZE, 0);
  recv(socket, year, BUFFER_SIZE, 0);
  recv(socket, month, BUFFER_SIZE, 0);
  recv(socket, day, BUFFER_SIZE, 0);
  recv(socket, photoName, BUFFER_SIZE, 0);
  // Reception of the photo send by client
  recv_photo(socket, tmp);
  // Creation of the directory tree
  creat_directory(&path, username, year, month, day);
  // Recording of the photo
  recording_photo(path, photoName, tmp);
}

int main(int argc, char *argv[])
{
  int sockfd, new_fd;
  // listen on sock_fd, new connection on new_fd
  struct sockaddr_in my_addr;
  // my address information
  struct sockaddr_in their_addr;
  // connector's address information
  unsigned int sin_size;
  int yes = 1;

  if (argc != 2)
  {
    fprintf(stderr, "Donner le chemin vers pool_v2");
    return EXIT_FAILURE;
  }

  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
  {
    perror("Serveur: socket");
    return EXIT_FAILURE;
  }

  if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
  {
    perror("Serveur: setsockopt");
    return EXIT_FAILURE;
  }

  my_addr.sin_family = AF_INET;
  my_addr.sin_port = htons(PORT);
  my_addr.sin_addr.s_addr = INADDR_ANY;

  if (bind(sockfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) == -1)
  {
    perror("Serveur: bind");
    return EXIT_FAILURE;
  }

  if (listen(sockfd, BACKLOG) == -1)
  {
    perror("Serveur: listen");
    return EXIT_FAILURE;
  }

  sin_size = sizeof(struct sockaddr_in);

  while (1)
  {

    if ((new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size)) == -1)
    {
      perror("Serveur: accept");
    }

    printf("Serveur:  connection du client %s\n", inet_ntoa(their_addr.sin_addr));

    if (fork() == 0)
    {
      /* this is the child process */
      close(sockfd);
      // receive all data
      recv_protocol(new_fd, argv[1]);
      send(new_fd, "0", 1, 0);

      return EXIT_SUCCESS;
    }

    close(new_fd);
  }

  return EXIT_SUCCESS;
}
