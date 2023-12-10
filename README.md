# ext2-File System
This is a lightweight ext2 file system that uses a TCP connection for communication. The file system mimics the UNIX file system.
Features:
- TCP communication (UNIX-like shell -> file system)
- Storage of up to 2GB
- Transfer files from and to the host system from the file system
- Lightweight user authentication with up to 6 users (5 regular users, 1 super user (root))
- Shell that recognizes commands, filenames, and paths
- Available commands:
     ls – provide the equivalent of a “ls -l” long output of the current directory.
     cd – change directory to the given directory
     mkdir – make a directory with the name given.
     lcp – copy a host file to the current directory in the FS
     Lcp – copy a FS file from the current directory to the current directory in the host system
     shutdown – shutdown and terminate the FS and the shell.
     exit – causes the Shell to terminate, allowing the FS to make a connection to yet another shell
     chown - change ower of files
     rmdir - remove directories
     cp – copy one or more FS files to a destination. When only one file is being copied, the destination can be either a filename or a directory name.
     mv – similar to cp, but moves the file(s)
     rm – removes one or more files
     ln – create a hard link
     cat – print the contents of one or more files to the screen
