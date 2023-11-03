#ifndef fs_h
#define fs_h
#include <fstream>

//Datas
#define BLOCK_SIZE 4096
#define TOTAL_BLOCK_NUM 4096
#define TOTAL_INODE_NUM 1024


class FileSystem
{

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
            char Mode[12];
            int block_count;
            int link_count;
            int uid;
            int gid;
            int file_size;
            char creation_time[14];
            char modified_time[14];
            char read_time[14];
            int direct_block_pointers[12];
            int indirect_block_address;
        };

        struct Inode inodeArray[TOTAL_INODE_NUM];        

        struct iNodeBitmap{
            char imap[4096];
        };

        struct blockBitmap{
            char bmap[4096];
        };

        // users
        struct User
        {
            std::string permission;
        };

        FileSystem();
        ~FileSystem();

        bool check_disk();                    // check if disk exists

        int inode_lookup();                   // search for free inode

        void initialize_inode(Inode &inode);  // initialize new inode with generic values


        void initialize_File_System();        // initialize disk with bitmaps, and inodes
        void write_to_disk(auto x, int len, int blockNum);  // write block with text "str" on disk
        int read_disk(auto &x, int blockNum); // read inode with pointer[0] = str from the list of inodes on the disk
        void readInode(Inode &i, int inodeNum);
        void updateInode(Inode i, int inodeNum);
        void ps(); //Just for testing
};

#endif
