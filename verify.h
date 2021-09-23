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
#include <map>
#include "./MD5.h"
using namespace std;

class Verify
{
public:
    //verify=need verified file's name
    void writeVerifyInfo(string verify);
    //fileDir=need verified dir's name. dest=verify info file's path
    void writeDirVerifyInfo(string fileDir);

    //use verify file to verify a file or a dir
    bool verify(string verifyFile,string file);

    bool VerifyDir(string file);

    bool VerifySingleFile(string file);

    void readVerifyFile(string file);

    Verify();

private:
    CMessageDigestAlgorithm5 MD5;
    int fd;
    map<string, string> fileMD5;
};