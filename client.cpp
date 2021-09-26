#include "client.h"

int port = 50836;

bool client::socketInit()
{
    sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("1.15.63.103");
    addr.sin_port = htons(port);

    if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)))
    {
        perror("connect:");
        return false;
    }
    return true;
}

bool client::logIn(string user, string password)
{
    char sig='1';
    write(sock, &sig, 1);
    char buf;
    int len = user.length();
    write(sock, &len, 4);
    write(sock, user.c_str(), user.length());
    len = password.length();
    write(sock, &len, 4);
    write(sock, password.c_str(), password.length());
    read(sock, &buf, 1);
    if(password!="77777")
           return false;
    if (buf = '0')
    {
        hasLogIn = true;
        return true;
    }
    else
        return false;
}

void client::sendFile(string src)
{
    char sig = '2';
    write(sock, &sig, 1);
    int fd = open(src.c_str(), O_RDWR);
    char buf[512];
    int size = 0;
    struct stat st;
    stat(src.c_str(), &st);
    int tot = st.st_size;
    write(sock, &tot, sizeof(int));

    while ((size = read(fd, buf, 512)) > 0)
    {
        write(sock, buf, size);
    }
    return;
}

void client::receiveFile(string dest)
{
    int fd = open(dest.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0777);
    char buf[512];
    int size = 0;

    char sig = '3';
    write(sock, &sig, 1);

    int totalSize = 0;
    read(sock, &totalSize, sizeof(int));

    while (totalSize > 0 && ((size = read(sock, buf, 512)) > 0))
    {
        totalSize -= size;
        write(fd, buf, size);
    }
    return;
}

void client::logOut()
{
    hasLogIn = false;
    char sig = '0';
    write(sock, &sig, 1);
    close(sock);
}

client::~client()
{
    if (hasLogIn == true)
        logOut();
}
