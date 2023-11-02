#ifndef fs_h
#define fs_h
#include <fstream>

class FileSystem
{
private:
    int block_bitmap[10];
    int inode_bitmap[10];

public:
    // block
    struct Block
    {
        char text[4096];
    };

    // inode
    struct Inode
    {
        // meta data
        std::string Mode;
        int block_count;
        int link_count;
        int uid;
        int gid;
        int file_size;
        std::string creation_time;
        std::string modified_time;
        std::string read_time;
        //

        int block_pointers[100]; // 61,917,364,224 block pointers
    };

    // users
    struct User
    {
        std::string permission;
    };

    Inode inodeList[10];

    FileSystem();

    bool check_disk();                    // check if disk exists
    int inode_lookup();                   // search for free inode
    void initialize_inode(Inode &inode);  // initialize new inode with generic values
    void initialize_File_System();        // initialize disk with bitmaps, and inodes
    void write_to_disk(std::string str);  // write block with text "str" on disk
    void read_from_disk(std::string str); // read inode with pointer[0] = str from the list of inodes on the disk
};

#endif