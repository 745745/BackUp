#include <string>
#include "./copy.h"
#include "./pack.h"
#include "./Encryption.h"
#include"./verify.h"
using namespace std;

int main()
{
   Verify a;
   a.writeDirVerifyInfo("../code");
   bool t=a.verify("./Verify.ver","../code");
   if(t)
      cout<<"verify pass";
}

/*



   Encryption a;
   a.encryption("./test_case/4","./test_case/1");
   a.decryption("./test_case/5","./test_case/4");

   //test for packFile function.
   packFile p;
   p.addFileFromDir("../api_test");
   p.loadAllFile();
   p.saveAsFile("../");
   p.unpack("../test","../packFile.wzy");


      Encryption a;
   unsigned char key[16];
   memcpy(key,"1594261537264837",16);
   unsigned char content[16];
   memcpy(content,"1594261537264837",16);
   a.setKey(key);
   a.encryp(content);
   a.decryp(content);
   cout<<content;
*/