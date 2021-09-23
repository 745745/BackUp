#include <sys/socket.h>
#include<sys/types.h>
#include <netinet/in.h>
#include<arpa/inet.h>
#include <unistd.h>
#include <vector>
#include <string>
#include<string.h>
#include <fcntl.h>
using namespace std;

class client
{
public:
    bool logIn(string user, string password);
    vector<string> getFileList();
    void sendFile(string src);
    void receiveFile(string dest);
    bool socketInit();
private:
    bool hasLogIn;
    int sock;
};