#include <stdio.h>
#ifdef _WIN32
#include <winsock2.h>
#endif
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <fcntl.h>  // for open
#include <unistd.h> // for close
#include <netdb.h>
#include <string.h>
#include "UI.h"
#include "clientP2P.h"

#define MAX 80
#define SA struct sockaddr
#define clear() printf("\033[H\033[J")



void hostPerson(int sockfd, int typeOfGame, char name[], int connectserver)
{
  strcpy(pointBroad, "000000000");
  char buff[MAX], msg[MAX], competitorName[MAX];
  int n;

  bzero(buff, MAX);
  bzero(competitorName, MAX);

  if (typeOfGame == 2)
  {
    strcpy(buff, name);
    send(sockfd, buff, sizeof(buff), 0);
    recv(sockfd, competitorName, sizeof(competitorName), 0);
  }
  // infinite loop for game

  for (;;)
  {
    bzero(buff, MAX);
    clear();

    ingame(pointBroad);
    if (typeOfGame == 2)
      score(name, competitorName);
    printf("Doi doi phuong danh ...\n");
    // read the message from client and copy it in buffer
    if (recv(sockfd, buff, sizeof(buff), 0) == 0)
    {
      if (typeOfGame == 2)
      {
        bzero(msg, MAX);
        strcat(msg, "6~");
        strcat(msg, name);
        strcat(msg, "~");
        strcat(msg, competitorName);

        send(connectserver, msg, sizeof(msg), 0);
        recv(connectserver, msg, sizeof(msg), 0);

        bzero(msg, MAX);
        printf("%s", msg);
      }
      printf("\n Ban da chien thang do nguoi choi kia da bo cuoc !!!");
      getchar();
      close(sockfd);
      break;
    }

    // print buffer which contains the client contents
    strcpy(pointBroad, updateBroad(pointBroad, buff, '2'));
    clear();
    ingame(pointBroad);
    if (typeOfGame == 2)
      score(name, competitorName);

    if (checkDraw()) {
      printf("Ket qua hoa!");
      getchar();
      close(sockfd);
      break;
    }

    if (checkWinner(pointBroad, '2'))
    {
      bzero(buff, MAX);
      printf("Ban da thua cuoc !!!");

      getchar();
      close(sockfd);
      break;
    }
    // copy server message in the buffer
    do {
      printf("Nhap vi tri muon danh : ");
      bzero(buff, MAX);

      n = 0;
      while ((buff[n++] = getchar()) != '\n')
        ;

      if (isPositionExits(buff))
        printf("Vi tri da ton tai!\n");
      else if (!isValidMove(buff))
        printf("Vi tri khong hop le!\n");
    } while(isPositionExits(buff) || !isValidMove(buff));

    if (strcmp(buff, "q\n") == 0)
    {
      close(sockfd);
      break;
    }
    // and send that buffer to client
    send(sockfd, buff, sizeof(buff), 0);
    strcpy(pointBroad, updateBroad(pointBroad, buff, '1'));
    if (checkWinner(pointBroad, '1'))
    {
      printf("Ban da chien thang !!!");
      if (typeOfGame == 2)
      {
        bzero(msg, MAX);
        strcat(msg, "6~");
        strcat(msg, name);
        strcat(msg, "~");
        strcat(msg, competitorName);

        send(connectserver, msg, sizeof(msg), 0);
        recv(connectserver, msg, sizeof(msg), 0);

        bzero(msg, MAX);
        printf("%s", msg);
      }
      getchar();
      close(sockfd);
      break;
    }
  }
}

void startP2P(char ip[], int PORT, int typeOfGame, char name[], int connectserver)
{
  int sockfd, connfd, len;
  struct sockaddr_in servaddr, cli;

  // socket create and verification
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd == -1)
  {
    printf("socket creation failed...\n");
    exit(0);
  }
  else
    printf("Socket successfully created..\n");
  bzero(&servaddr, sizeof(servaddr));

  // assign IP, PORT
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = inet_addr(ip);
  servaddr.sin_port = htons(PORT);

  printf("ip port: %s:%d___", ip, PORT);

  // Binding newly created socket to given IP and verification
  if ((bind(sockfd, (SA *)&servaddr, sizeof(servaddr))) != 0)
  {
    printf("socket bind failed...\n");
    exit(0);
  }
  else
    printf("Socket successfully binded..\n");

  // Now server is ready to listen and verification
  if ((listen(sockfd, 5)) != 0)
  {
    printf("Listen failed...\n");
    exit(0);
  }
  else
    printf("Waiting another player ...\n");
  len = sizeof(cli);

  // Accept the data packet from client and verification
  connfd = accept(sockfd, (SA *)&cli, &len);
  if (connfd < 0)
  {
    printf("server acccept failed...\n");
    exit(0);
  }
  else
    printf("server acccept the client...\n");

  // Function for chatting between client and server
  // typeOfGame ==1 -> nomarl game
  // typeOfGame ==2 -> rank game
  hostPerson(connfd, typeOfGame, name, connectserver);

  close(sockfd);
}
