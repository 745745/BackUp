#include"compress.h"


void CompressFile::compressFile(string dest,string file,string password)
{
    struct stat st;
    stat(file.c_str(),&st);
    int len=st.st_size;
    buf=new char[len];



}