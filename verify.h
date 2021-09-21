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
#include"./MD5.h"
using namespace std;

class Verify
{
    public:
    //verify=need verified file's name 
    void writeVerifyInfo(int fd,string verify);
    //fileDir=need verified dir's name. dest=verify info file's path
    void writeVerifyInfo(string fileDir,string dest);

    //use verify file to verify a file or a dir
    bool VerifyFiles(string file,string verifyFile);

    private:
    CMessageDigestAlgorithm5 MD5;
};