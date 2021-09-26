#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <sys/unistd.h>
#include <string>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <utime.h>
#include <vector>
#include <map>
#include <limits.h>



using namespace std;

#define REGULAR '0'
#define SymLNK '1'
#define DIRENT '2'
#define FIFO '3'
#define HARDLNK '4'


const char magic[] = "Recordh";
union Record
{
    struct head
    {
        char name[100];
        char linkName[100];
        char modTime[8];
        char actTime[8];
        char mode[4];
        char uid[4];
        char gid[4];
        char size[8];
        char magic[8];
        char type[1];
    };

    char block[512];
};

class packFile
{
public:
    vector<string> file;         //the file that pack contians
    vector<char *> blocks;       //contents including header and block
    map<int, string> inode_name; //to avoid copy hard link multiple times

    string findInode(int inode);
    void addInode(int inode, string name);
    void readFile(string src);
    void addFile(string src);
    void loadAllFile();
    int addFileFromDir(string dir);
    void saveAsFile(string dest);

    char *setHead(string src);
    void recoverFromPack(string pack);
    void unpack(string dest, string packSrc);
    //vector<string> getFileListFromPack(string src);
    bool verify(string src,string packfile);
};
