#include <stdio.h>
#define SPACE 18
#define NULL 19
#define INTERFACE 20
#define INPUT 21
#define OUTPUT 22
#define VARS 23
#define ATOM 24
#define ENDATOM 25
#define ENDINTERFACE 26
#define NOMATCH 27
#define INPUTATOM 28
#define  OUTPUTATOM 29
#define  NOSTATE 30

char* splitIntf(char* );
int parseInput(FILE*, char* );
void writeIO(FILE* , const char* );
int myfscanf(FILE* , char* );

