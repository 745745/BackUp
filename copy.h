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
#include<map>
#include <mutex>
#include"/home/wzy/api_test/verify.h"

using namespace std;


int getFileMode(struct stat *st);
int setFileMode(char *path, int mode);
int setFileOwner(char *dest, struct stat *st);
int setFileTime(char *dest, struct stat *st);
int setFileAttribute(char *dest, struct stat *st);
string getFileName(string src);
int copyFile(string dest, string src);
int copyDir(string dest, string src);
int copyLink(string dest, string src);
int copyPipe(string dest, string src);
int copy(bool init,string dest, string src);
vector<string> copySelectFile(string dest, vector<string> fileName);
