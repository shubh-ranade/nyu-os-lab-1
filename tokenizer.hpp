#ifndef TOKENIZER_HPP
#define TOKENIZER_HPP

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<iostream>
#include<fstream>

#define BUFFER_SIZE 4096
#define MAX_NUM 1 << 30

extern int linenum;
extern int lineofs;
extern int line_end;
extern bool eof;
extern bool parse_error;

char* getToken(std::ifstream&);

//errors
extern void __parseerror(int);

//functions to be used by the linker
int readInt(std::ifstream&);
char* readSymbol(std::ifstream&);
char readIAER(std::ifstream&);

#endif