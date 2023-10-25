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

inodeList::inodeList(int size)
{
    this->bm = new int[size * size];

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

inodeList::~inodeList()
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

int inodeList::free_inode_lookup()
{
    int rc = 0;
    for (int i = 0; i < 32 * 32; i++)
    {
        if (bm[i] == 0)
        {
            rc = i;
            break;
        }
    }

    return rc;
}

void inodeList::create_inode(inode *new_inode)
{
    int rc = free_inode_lookup();
    int row = rc / 32;
    int column = rc % 32;
    if (rc == -1)
    {
        cout << "All inodes are occupied" << endl;
        return;
    }

    inodes[row][column] = *new_inode;
    bm[rc] = 1;
}

inode *inodeList::Search_inode_from_table(int num)
{
    inode *rc = NULL;

    for (int i = 0; i < 32; i++)
    {
        for (int j = 0; j < 32; j++)
        {
            if (inodes[i][j].inode_num == num)
            {
                rc = &inodes[i][j];
                break;
            }
        }
    }

    return rc;
}

int main()
{
    inode *node1 = new inode();
    node1->Mode = "file 1";
    node1->inode_num = 1;

    inodeList *it = new inodeList(32);

    it->create_inode(node1);
    // for (int i = 0; i < 32; i++)
    // {
    //     for (int j = 0; j < 32; j++)
    //     {
    //         cout << it->bitmap[i][j];
    //     }
    //     cout << endl;
    // }
    // int i = 0;
    // while (i < 32 * 32)
    // {
    //     cout << it->bm[i];

    //     if (i != 0 && i % 32 == 0)
    //     {
    //         cout << endl;
    //     }
    //     i++;
    // }

    inode *node2 = new inode();
    node2->Mode = "file 2";
    node2->inode_num = 2;
    it->create_inode(node2);

    // cout << "\n"
    //      << endl;
    // i = 0;
    // while (i < 32 * 32)
    // {
    //     cout << it->bm[i];

    //     if (i != 0 && i % 32 == 0)
    //     {
    //         cout << endl;
    //     }
    //     i++;
    // }

    // cout << endl;

    inode *wanted = it->Search_inode_from_table(2);
    cout << wanted->Mode << endl;
}