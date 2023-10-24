#include <iostream>
#include <ctime>
#include "inode.h"
using namespace std;

inode::inode()
{
    // meta data
    string Mode = "";
    int inode_num = 0;
    int link_count = 0;
    int uid = 0;
    int gid = 0;
    int file_size = 0;
    time_t creation_time = chrono::system_clock::to_time_t(chrono::system_clock::now()); // ctime(&creation_time) to output time
    time_t modified_time = chrono::system_clock::to_time_t(chrono::system_clock::now());
    time_t read_time = chrono::system_clock::to_time_t(chrono::system_clock::now());
    //

    this->block_pointers = new int[12]; // direct pointers
    this->single_indirect = new int *[12];
    for (int i = 0; i < 12; i++)
    {
        this->single_indirect[i] = new int[12];
    }

    this->double_indirect = new int **[12];
    for (int i = 0; i < 12; i++)
    {
        this->double_indirect[i] = new int *[12];
        for (int j = 0; j < 12; j++)
        {
            this->double_indirect[i][j] = new int[12];
        }
    }

    this->triple_indirect = new int ***[12];
    for (int i = 0; i < 12; i++)
    {
        this->triple_indirect[i] = new int **[12];
        for (int j = 0; j < 12; j++)
        {
            this->triple_indirect[i][j] = new int *[12];
            for (int k = 0; k < 12; k++)
            {
                this->triple_indirect[i][j][k] = new int[12];
            }
        }
    }
}

inode::~inode()
{
    delete[] block_pointers; // destruct block_pointers
    for (int i = 0; i < 12; i++)
    {
        delete[] this->single_indirect[i];
    }

    // destruct double_indirect at every level
    for (int i = 0; i < 12; i++)
    {
        delete[] this->double_indirect[i];
        for (int j = 0; j < 12; j++)
        {
            delete[] this->double_indirect[i][j];
        }
    }

    // destruct triple_indirect at every level
    for (int i = 0; i < 12; i++)
    {
        delete[] this->triple_indirect[i];
        for (int j = 0; j < 12; j++)
        {
            delete[] this->triple_indirect[i][j];
            for (int k = 0; k < 12; k++)
            {
                delete[] this->triple_indirect[i][j][k];
            }
        }
    }
}

inodeTable::inodeTable(int size)
{
    this->bitmap = new int *[size];
    for (int i = 0; i < size; i++)
    {
        this->bitmap[i] = new int[size];
    }

    this->inodes = new inode *[size];
    for (int i = 0; i < size; i++)
    {
        this->inodes[i] = new inode();
    }
}

inodeTable::~inodeTable()
{
    // destruct bitmap at every level
    for (int i = 0; i < 32; i++)
    {
        delete[] this->bitmap[i];
    }

    // destruct inodes at every level
    for (int i = 0; i < 32; i++)
    {
        delete[] this->inodes[i];
    }
}

int *inodeTable::inode_lookup()
{
    int *rc = new int[2];
    rc[0] = -1;
    rc[1] = -1;

    for (int i = 0; i < 32; i++)
    {
        for (int j = 0; j < 32; j++)
        {
            if (bitmap[i][j] == 0)
            {
                rc[0] = i;
                rc[1] = j;
                return rc;
            }
        }
    }

    return rc;
}

void inodeTable::create_inode(inode *new_inode)
{
    int *rc = inode_lookup();

    int i = rc[0];
    int j = rc[1];
    if (i == -1 && j == -1)
    {
        cout << "All inodes are occupied" << endl;
        return;
    }

    inodes[i][j] = *new_inode;
}

int main()
{
    inode *node1 = new inode();
    node1->Mode = "node 1";

    inodeTable *it = new inodeTable(32);

    cout << it->inode_lookup()[0] << " " << it->inode_lookup()[1] << endl;

    it->create_inode(node1);
    cout << it->inode_lookup()[0] << " " << it->inode_lookup()[1] << endl;

    inode *node2 = new inode();
    node2->Mode = "node 2";
    it->create_inode(node2);
    cout << it->inode_lookup()[0] << " " << it->inode_lookup()[1] << endl;
}