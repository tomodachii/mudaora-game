#ifndef __HANDLE_H__
#define __HANDLE_H__
   
char **words(char *line, int *total, char *strCut);
int isNumber(char *str);
int isIpV4(char *str);
void clearBuffer();
void addToken(char *str, SignalState signal);
 
#endif