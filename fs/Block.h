// Name: Group 6
// Assignment: OS KPA
// Class: CSC351 Fall 2023

#ifndef Block_h
#define Block_h
//include for the use of NULL
#include <cstdlib>

//******************************************************************************

// node to represent a chunk of memory
class node{
    private:

        //start and end of the memory chunk
        int start;
        int end;
        //process id of the memory chunk
        int pid;
        //pointer to the next node
        node *next;
        

        node(int start = 0, int end = 0, int pid =-1, node *next = NULL);

        friend class memoryLinkedList;
        friend class firstFit;       
};

//******************************************************************************

class memoryLinkedList{
    private:
        //pointer that points to the first node
        node *first;
        //pointer that points to the last node
        node *last;

        //number of the elements in the list
        int count;
        //recursion helper for printIt function
        void printIt(node *p, int index)const;
        void clear(node *p);
        //recursion helper for find function
        int find(int val, node *p, int index)const;

    public:
        memoryLinkedList();
        ~memoryLinkedList();

        //Function to delete a node at given index
        void deleteAt(int index);

        //Function to add a node with the given values in the beginning
        void insert(int s, int e, int pid);

        //Function to add a node with the given values at given index
        void insertAt(int index, int s, int e, int pid);

        //function to find the index of the first node holding value s
        int find(int s)const;

        //function to print the index and the start and end of each nodes
        void printIt()const;

        //function to delete all the nodes
        void clear();

        //funciton to deallocate memory from used memory and put back in free memory
        int dealloc(int pid, memoryLinkedList *freeMem);

        //funciton to get the number of holes in free memory
        int fragment();

        friend class firstFit;
};

//******************************************************************************
//First Fit Technique finds the first memory chunk that is big enough
class firstFit{
    private:
        memoryLinkedList *freeMem;
        memoryLinkedList *usedMem;

    public:
        firstFit();
        //Function to allocate memory according to first fit technique
        int allocate_mem (int pid, int units);
        //Function to deallocate memory
        int deallocate_mem (int pid);
        //Function to get number of fragments
        int fragment_count();
        void printFreeMem();
        void printUsedMem();
};

#endif