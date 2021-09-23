#include"client.h"


bool client::socketInit()
{
    sock=socket(AF_INET,SOCK_STREAM,0);
    struct sockaddr_in addr;
    memset(&addr,0,sizeof(addr));
    addr.sin_family=AF_INET;
    addr.sin_addr.s_addr=inet_addr("127.0.0.1");
    addr.sin_port=htons(17902);

    
    if(connect(sock,(struct sockaddr*)&addr,sizeof(addr)))
    {
        perror("connect:");
        return false;
    }
    return true;
}


bool client::logIn(string user, string password)
{
    user+="\n";
    password+="\n";
    char buf;
    write(sock,user.c_str(),user.length());
    write(sock,password.c_str(),password.length());
    read(sock,&buf,1);

    if(buf='0')
    {
        hasLogIn=true;
        return true;
    }
    else
        return false;
}

void client::sendFile(string src)
{
    int fd=open(src.c_str(),O_RDWR);
    char buf[50];
    int size=0;
    while (size=read(fd,buf,50))
    {
        write(sock,buf,size);
    }
    return;
}

void client::receiveFile(string dest)
{
    int fd=open(dest.c_str(),O_WRONLY|O_CREAT|O_TRUNC);
    char buf[50];
    int size=0;
    
    write(sock,"request",8);

    int totalSize=0;
    read(sock,&totalSize,sizeof(totalSize));

    while(size=read(sock,buf,50) && totalSize>0)
    {
        totalSize-=size;
        write(fd,buf,size);
    }
    return;
}