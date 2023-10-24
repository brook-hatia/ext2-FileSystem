// Name: Group 6
// Assignment: OS KPA
// Class: CSC351 Fall 2023

#include "Block.h"
#include <cstdlib> //Include for the use of NULL
#include <iostream> //Include for the use of COUT
using namespace std; //For COUT

//******************************************************************************
//Constructor to initialize node
node::node(int start, int end, int pid, node *next){
    this->start = start;
    this->end = end;
    this->next = next;
    this->pid = pid;
}

//******************************************************************************
//Constructor to initialize memoryLinkedList
memoryLinkedList::memoryLinkedList(){
    first = NULL;
    last = NULL;
    count = 0;
}

//******************************************************************************
//deletes the dynamically allocated nodes
memoryLinkedList::~memoryLinkedList(){
    clear();
}

//******************************************************************************
//function to insert the value to the beginning of the list
void memoryLinkedList::insert(int s, int e, int pid){
    //create new node at beginning and set it as first
    //its next point to what first point to originally
    first = new node(s, e, pid, first);
    //check if the list is empty
    if (last == NULL) {
        //if true set new node also as last
        last = first;
    }

    count++;
}


//******************************************************************************
//function to insert given value at the given index
void memoryLinkedList::insertAt(int index, int s, int e, int pid){
   
    //check if index is valid
    if (index >= 0 && index <= count) {
        //if true node would be added successfully
       
        count++;
        node *p = new node(s, e, pid, NULL);
        //check if is inserting in the beginning
        if (index == 0) {
            //if true insert at beginning and point first to it
            p->next = first;
            first = p;

        }else{
            //if false point the new node's next to the previous node's next
            //and point the previous node's next to the new node
            node *r = first;
            //loop to find the node before the index
            for (int i =0; i<index-1; i++) {
            r = r->next;
            }
            p->next = r->next;
            r->next = p;
        }
        //check if the new node is at the end
        if (p->next == NULL) {
            //if true point last to it
            last = p;
        }
    }
}

//******************************************************************************
//Function to delete a node at given index
//and set &value as deleted value
void memoryLinkedList::deleteAt(int index){

    //check if index is valid
    if (index >= 0 && index < count) {
        //if true node would be deleted successfully

        count--;
        node *p = first;
        //check if is deleting the first node
        if (index == 0) {
            //if true get its value
            //and set its next as first
            first = p->next;

            //check if first now is NULL or is the last node
            if (first == NULL||first->next == NULL) {
                last = first;
            }
            //delete the node
            delete p;
            p = NULL;

        }else{
            //if false loop to find the node before the index
            for (int i =0; i<index-1; i++) {
                p = p->next;
            }
            //get the deleting node
            node *r = p->next;
            //point the next of the node before the index
            //to the deleting node's next
            p->next = r->next;
            //delete the node
            delete r;
            r = NULL;
            //check if the node before the index is now the last node
            if (p->next == NULL) {
                last = p;
                }
            }
        }
}

//******************************************************************************
//function to delete all the nodes
void memoryLinkedList::clear(node *p){
    //checking if reached end of list
    if (p) {
        //Recurse until end of list
        clear(p->next);
        //deleting the pointer that points to each node
        delete p;
        p = NULL;
    }
}

//******************************************************************************
//function to call recurssion helper clear
void memoryLinkedList::clear(){
    clear(first);
    count = 0;
    first = last = NULL;
}


//******************************************************************************
//function to return the position at which val was found
int memoryLinkedList::find(int s, node *p, int index)const{
    int position = -1;
    //check if reached the end of list
    if (p) {
        //check if the value is found
        if (p->start == s) {
            //get the index and end recurssion
            position = index;
        }else{
            //if false recurse until reached end of list or value found
            index++;
            position = find(s, p->next, index);
        }
    }
    return position;
}

//******************************************************************************
//function to call recurssion helper find
int memoryLinkedList::find(int s)const{
    return find(s, first, 0);
}
 
//******************************************************************************
//function to print the index and the value of each nodes
void memoryLinkedList::printIt(node *p, int index)const{
    //check reached end of list
    if (p) {
        cout << "At index " << index << " there is " << "Start: " << p->start << " and End: " << p->end << " and pid: " << p->pid << endl;
        index++;
        //recurse until reach end of list
        printIt(p->next, index);
    }
}

//******************************************************************************
//function to call recurssion helper printIt
void memoryLinkedList::printIt()const{
    //check if there is node in list
    if (count > 0) {
        printIt(first,0);
    }
}

//******************************************************************************
//funciton to deallocate memory from used memory and put back in free memory
int memoryLinkedList::dealloc(int pid, memoryLinkedList *freeMem){
    int rc = -1;
    node *p = first;
    node *q = freeMem->first;

    //loop to find the pid that matches and deallocate the memory
    for(int i = 0; i < count; i++){
        if(p->pid == pid){
            //if found put memory chunk from used memory back to free memory          
            //check if there is nothing thing in free memory
            if(freeMem->count == 0){
                //if so just insert into free memory
                freeMem->insert(p->start, p->end, p->pid);
                //set rc and remove memory chunk from used memory
                rc = 1;
                deleteAt(i);
            } else {
                //if there is more than 0 in free memory
                //loop to find the correct place for the memory chunk 
                for(int j = 0; j < freeMem->count; j++){  
                    //if found insert into the correct place in free memory            
                    if(q->start > p->end){                 
                        freeMem->insertAt(j, p->start, p->end, p->pid);
                        //set rc and remove memory chunk from used memory
                        rc = 1;
                        deleteAt(i);
                        break;
                    } else if (p->start > q->end){
                        if(q->next == NULL){
                            freeMem->insertAt(j+1, p->start, p->end, p->pid);                 
                            //set rc and remove memory chunk from used memory
                            rc = 1;
                            deleteAt(i);
                            break;
                        } else {
                            if(q->next->start > p->end){
                            freeMem->insertAt(j+1, p->start, p->end, p->pid);                 
                            //set rc and remove memory chunk from used memory
                            rc = 1;
                            deleteAt(i);
                            break;
                            }
                        }

                    }
                    q=q->next;
                }
                node *r = freeMem->first; 
                int index = 0;
                //check after putting back can any memory be merged
                //while not at the end of the free memory
                while(r->next){
                    //check if the neighbour memory chunk could be merged
                    if(r->next->start - r->end == 1){
                        //if true then merge
                        r->end = r->next->end;
                        freeMem->deleteAt(index+1);
                        if(r->next == NULL){
                            break;
                        }
                    }else{
                        r=r->next;
                        index++;
                    }   
                }
            }
            break;            
        }
        p = p->next;
    }
    return rc;
}

//******************************************************************************
//funciton to get the number of fragments in free memory
int memoryLinkedList::fragment(){
    int rc = 0;
    node *p = first;

    //Loop through the free memory
    for(int i = 0; i < count; i++){
        //increment rc when there is a hole in free memory
        if(p->end - p->start <= 2){
            rc++;
        }
        p=p->next;
    }

    return rc;
}

//******************************************************************************
//******************************************************************************
//Constructor for firstFit
firstFit::firstFit(){
    freeMem = new memoryLinkedList();
    freeMem->insert(0, 127, -1);
    usedMem = new memoryLinkedList();
}

//******************************************************************************
//Function to allocate memory according to first fit technique
int firstFit::allocate_mem (int pid, int units){
    int rc = -1;
    //Process may request 1-16 units of memory
    if(units >=1 and units <= 16){
        node *p = freeMem->first;

        //Check if there is still memory in free memory
        if(p){
            //loop through the free memory list to find the first memory chunk
            //that is big enough to allocate the units
            for(int i = 1; i <= freeMem->count; i++){
                //Check if memory chunk is greter than units
                if(p->end - p->start + 1 > units){
                    int newEnd = p->start + units;
                    //If found, allocate that chunk of memory to the used memory list
                    usedMem->insert(p->start, newEnd-1, pid);
                    //update the free memory list
                    p->start = newEnd;
                    //get the node traversed starting from 1
                    rc = i;
                    break;

                //Check if memory chunk is equal to units   
                }else if(p->end - p->start + 1 == units){
                    //If found, allocate that chunk of memory to the used memory 
                    //and delete it from the free memory
                    usedMem->insert(p->start, p->end, pid);
                    freeMem->deleteAt(i-1);
                    break;
                }
                p = p->next;
            }
        }
    }
    return rc;
}


//******************************************************************************
//function to deallocate memory for first fit
int firstFit::deallocate_mem (int pid){
    return usedMem->dealloc(pid, freeMem);
}

//******************************************************************************
//Function to get number of holes for first fit
int firstFit::fragment_count(){
    return freeMem->fragment();
}

//Function to check the nodes for each memory chunk
void firstFit::printFreeMem(){
    cout << "Free Memory list: " << endl;
    freeMem->printIt();
}

void firstFit::printUsedMem(){
    cout << "Used Memory list: " << endl;
    usedMem->printIt();
}