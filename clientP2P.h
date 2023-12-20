#ifndef __FILE_H__
#define __FILE_H__

extern char pointBroad[10];

char *genPort();
char *return_ip(char ip_port[]);
char *return_port(char ip_port[]);
int isPositionExits(char location[]);
int isValidMove(char location[]);
char *updateBroad(char pointBroad[], char location[], char player);
int checkWinner(char broad[], char xo);
int checkDraw();

#endif
