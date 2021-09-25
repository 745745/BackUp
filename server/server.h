#include <sys/socket.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <sys/stat.h>
#include<stdlib.h>
#include <netinet/in.h>
#include <sys/shm.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include <vector>
#include <string>
#include <string.h>
#include <map>
#include <fcntl.h>
#include <iostream>

using namespace std;
class server
{
public:
    void logInRequest();
    void logOutRequest();
    void FileRequest();
    void ReceiveFile();
    void sockInit();

    void chooseFunction();
    bool listenSocket();
    void connectClient();

    bool verifyID(string user, string password);
    bool disconnect();

    void closeSock();
    server();
private:
    int sock;
    int connectSock;
    string userName="";
};