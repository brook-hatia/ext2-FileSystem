#ifndef fs_h
#define fs_h
#include <fstream>
#include <string.h>
using namespace std;

// Datas
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

    // struct Block blocks[10];

    // Size is exactly 128
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

    // size is exactly 256
    struct directoryEntry
    {
        char *name; //name must be atmost 250
        int inodeNumber;
    };

    // Array of Inode
    struct Inode inodeArray[TOTAL_INODE_NUM];

    struct iNodeBitmap
    {
        char imap[1024];
    };

    struct blockBitmap
    {
        char bmap[4096];
    };

    // Size is exactly 4096
    struct directory
    {
        struct directoryEntry dirEntries[16];
    };

    struct User
    {
        string name[6];
        int uid[6];
    };

    struct File {
        char *name;
        int uid;
        int inode_number;
    };

    // Variables
    blockBitmap bm; // block bit map
    iNodeBitmap im; // inode bit map

    directory wd; // working directory
    directory rd; // root directory

    User curr_user;

    string cwd;

    struct directory directoryTable[10]; // Table recording the opened directories

    int currentDirectoryBlock; // first block of the current directory
    int initDataBlock;         // first block for file data
    int atRoot;
    int current_user; // stores uid of signed-in user
    long file_size; //size of a file to be read

    // Functions
    FileSystem();
    ~FileSystem();

    bool check_disk(); // check if disk exists

    int get_free_inode(); // search for free inode

    void initialize_inode(Inode &inode, int uid, int linkCount, int fileSize, string mode, int creTime, int modTime, int reTime); // initialize new inode with generic values
    int get_eight_free_block();                                                                                                   // search for 8 free blocks
    int get_free_block();                                                                                                         // search for free block
    void terminate_File_System();                                                                                                 // stores everything back to disk
    void initialize_File_System();                                                                                                // initialize disk with bitmaps, and inodes
    template <typename T>
    void write_to_disk(T x, int len, int blockNum); // write block with text "str" on disk
    template <typename T>
    void read_disk(T &x, int blockNum); // read inode with pointer[0] = str from the list of inodes on the disk
    void readInode(Inode &i, int inodeNum);
    void updateInode(Inode i, int inodeNum);
    int get_directory_block(directory &dir, int inodeNum); // Gets the directory and returns block number

    // functions
    int my_mkdir(string directoryName);
    int my_cd(string directoryName);
    string my_ls();
    int my_rmdir(string directoryName);
    int my_lcp(char *host_file);
    int my_Lcp(char *fs_file);

    int sign_in(string name); // searches user on disk and signs-in if user exists, else return -1;
    string who_am_i();        // returns name of signed-in user

    void ps(); // Just for testing

    // Server side code
    string *scan(char *parameter);            // identify function names from filenames/paths
    string identify_function(string *prompt); // call appropriate functions from prompt
    void start_server();
};

#endif