#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <vector>
#include <string>
#include <fcntl.h>
using namespace std;

class client
{
public:
    bool logIn(string user, string password);
    vector<string> getFileList();
    void sendFile(string src);
    void receiveFile(string dest);

private:
    bool hasLogIn;
};