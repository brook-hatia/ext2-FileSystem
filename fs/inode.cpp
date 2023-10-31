#include <iostream>
#include <ctime>
#include "inode.h"
#include <fstream>
#include <string>
using namespace std;

inode::inode()
{
    // meta data
    string Mode = "";
    int block_count = 0;
    int link_count = 0;
    int uid = 0;
    int gid = 0;
    int file_size = 0;
    std::string creation_time = "";
    std::string modified_time = "";
    std::string read_time = "";

    this->block_pointers = new int[12];
    for (int i = 0; i < 12; i++)
    {
        block_pointers[i] = -1; // direct pointers initialized to -1(invalid position) because 0 is a valid position
    }

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

inodeList::inodeList()
{
    this->bitmap = new int[32 * 32];
}

inodeList::~inodeList()
{
    // destruct bitmap
    delete[] this->bitmap;
}

// search free bit, return its index, and change the bit to a 1
int inodeList::inode_lookup()
{
    int rc = -1;

    fstream file("disk", ios::in | ios::out); // read/write on disk
    if (!file.is_open())
    {
        cout << "Couldn't open file";
    }
    else
    {
        // read back the inode bitmap from disk
        int temp_bitmap[1024] = {0};
        file.read((char *)&temp_bitmap, sizeof(temp_bitmap));

        // look for a free bit (bit = 0)
        for (int i = 0; i < 1024; i++)
        {
            if (temp_bitmap[i] == 0)
            {
                rc = i;
                int data = 1;
                file.seekp(i * sizeof(int));
                file.write((char *)&temp_bitmap, sizeof(temp_bitmap));
                break;
            }
        }
    }

    return rc;
}