#include<string>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include<string.h>
#include<stdio.h>
using namespace std;


extern unsigned char sBox[];
extern unsigned char invsBox[];


class Encryption
{
public:
    unsigned char state[4][4];
    unsigned char k[4][4];//key
    unsigned char w[11][4][4];
    int encryption(string dest,string src);
    int decryption(string dest,string src);
    void SubBytes();
    void shiftRow();
    void MixCol();
    unsigned char FFMul(unsigned char a,unsigned char b);
    void addRoundKey(int i);
    void KeyExpansion();

    void InvSubBytes();
    void InvShiftRow();
    void InvMixCol();


    void encryp(unsigned char* input);
    void decryp(unsigned char* input);

    void setKey(unsigned char*  key);
    void setState(unsigned char* stat);

    int checkPadding(unsigned char* input);
    bool checkLastPadding(string src);


    //todo:make a key file to keep password.can chose by user
};