#ifndef LINKER_HPP
#define LINKER_HPP
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#define MOD_OPCODE 1000
#define MAX_OPCODE 9
#define MAX_OPERAND 999
#define MAX_IMMEDIATE 10000
#define MAX_USE_DEF 16
#define MACHINE_SIZE 512

typedef std::string Symbol;

//errors and warnings
extern void __warning(int, int, const char*, int, int);
extern void __linkererror(int, const char*);

void pass1(std::ifstream&);
void pass2(std::ifstream&);

#endif