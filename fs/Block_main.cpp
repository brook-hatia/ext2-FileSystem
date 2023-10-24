// Name: Group 6
// Assignment: OS KPA
// Class: CSC351 Fall 2023

#include "Block.h"
#include <iostream> //Include for the use of COUT
using namespace std; //For COUT and rand

firstFit blocklist;

int main(int argc, char const *argv[])
{
    blocklist.printFreeMem();
    blocklist.printUsedMem();
    blocklist.allocate_mem(1, 8);
    blocklist.printFreeMem();
    blocklist.printUsedMem();
}