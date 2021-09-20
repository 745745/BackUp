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
#include <mutex>
using namespace std;


class CompressFile
{
    //file content
    char* buf;

    map<char,string> encodeDic;

    //return compressed content
    char* setContent();

    //todo:compress
    void compressFile(string dest,string file,string password);
    void unzipFile(string dest,string file,string encode);
    void readDic(string file);
    void writeDic(string dest);
};