#include "server.h"
int port = 50836;
bool logout = true;

void handle(int sig)
{
    logout = false;
}

string removePrefix(string x)
{
    for (int i = 0; i < x.length(); i++)
    {
        if (x[i] == '.' && x[i + 1] != '.')
            return x.substr(i + 1);
    }
    return x;
}

void server::sockInit()
{
    sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("172.17.0.5");
    addr.sin_port = htons(port);

    if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) == -1)
    {
        perror("bind");
        close(sock);
        return;
    }

    char buf[512];
}

bool server::listenSocket()
{
    if (listen(sock, 20) == -1)
    {
        perror("listen");
        close(sock);
        return false;
    }
    return true;
}

void server::connectClient()
{
    struct sockaddr_in clientAddr;
    socklen_t len = sizeof(clientAddr);

    connectSock = accept(sock, (struct sockaddr *)&clientAddr, &len);
    if (connectSock < 0)
    {
        perror("connect");
        close(sock);
        return;
    }
    else
        cout << "connected!\n";
}

void server::chooseFunction()
{
    logout=true;
    char buf;
    alarm(5);
    int size = 0;
    while (size <= 0)
    {
        if (logout == false)
        {
            closeSock();
            return;
        }
        size = read(connectSock, &buf, 1);
    }
    alarm(0);

    switch (buf)
    {
    case '0':
        cout << "log out \n";
        logOutRequest();
        break;

    case '1':
        cout << "log request!\n";
        logInRequest();
        break;

    case '2':
        cout << "receive file!\n";
        ReceiveFile();
        break;
    case '3':
        cout << "File request!\n";
        FileRequest();
        break;
    case '4':
        break;

    default:
        break;
    }
}

void server::FileRequest()
{
    if (userName.length() == 0)
        return;

    string file = userName + "/" + "packFile.wzy";
    int fd = open(file.c_str(), O_RDONLY);
    struct stat st;
    stat(file.c_str(), &st);
    int totalSize = st.st_size;
    write(connectSock, &totalSize, sizeof(int));
    int size = 0;
    char buf[512];
    while (1)
    {
        size = read(fd, buf, 512);
        if (size <= 0)
            break;
        write(connectSock, buf, size);
    }
}

void server::ReceiveFile()
{
    if (userName.length() == 0)
        return;

    char buf[512];
    mkdir(("./" + userName).c_str(), 0777);
    string path = "packFile.wzy"; //./"+userName+'/'+name
    path = "./" + userName +'/'+ path;
    int fd = open(path.c_str(), O_CREAT | O_TRUNC | O_WRONLY, 0777);

    int size = 0;
    read(connectSock, &size, sizeof(int));
    if (size == 0)
        return;
    int readSize;
    while (size > 0)
    {
        if (size < 512)
            readSize = read(connectSock, buf, size);
        else
            readSize = read(connectSock, buf, 512);
        write(fd, buf, readSize);
        size -= readSize;
    }
}

void server::logInRequest()
{
    int len;
    read(connectSock, &len, 4);
    char *buf = (char *)malloc(len * sizeof(char));
    int s = read(connectSock, buf, len);
    string user(buf);
    free(buf);
    read(connectSock, &len, 4);
    buf = new char[len];
    read(connectSock, buf, len);
    string password(buf);

    if (verifyID(user, password))
    {
        userName = user;
        char sig = '0';
        write(connectSock, &sig, 1);
        return;
    }
    else
        return;
}

bool server::verifyID(string user, string password)
{
    return true;
}

bool server::disconnect()
{
    if (connectSock == 0)
        return true;
    else
        return false;
}

void server::closeSock()
{
    close(connectSock);
    connectSock = 0;
}

server::server()
{
    signal(SIGALRM, handle);
}

void server::logOutRequest()
{
    userName.clear();
    connectSock = 0;
}