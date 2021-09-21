#include"./verify.h"


void Verify::writeVerifyInfo(int fd,string verify)
{
    string md5=MD5.Encode(verify,true);
    write(fd,verify.c_str(),verify.length());
    write(fd," ",1);
    write(fd,md5.c_str(),md5.length());
    return;
}


void Verify::writeVerifyInfo(int fd,string verify)
{
    
}