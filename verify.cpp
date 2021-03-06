#include "./verify.h"

void Verify::writeVerifyInfo(string verify)
{
    char *path;
    path = realpath(verify.c_str(), nullptr);
    string p(path);
    struct stat st;
    stat(p.c_str(),&st);
    if(S_ISFIFO(st.st_mode))
    {
        write(fd, p.c_str(), p.length());
        write(fd, " ", 1);
        write(fd, " 12345",5 );
        write(fd, "\n", 1);
        return;
    }
    string md5 = MD5.Encode(p, true);
    write(fd, p.c_str(), p.length());
    write(fd, " ", 1);
    write(fd, md5.c_str(), md5.length());
    write(fd, "\n", 1);
    return;
}

void Verify::writeDirVerifyInfo(string fileDir)
{
    DIR *dir;
    struct dirent *d;
    struct stat st;
    dir = opendir(fileDir.c_str());
    while ((d = readdir(dir)) != nullptr)
    {
        //ignore "." and ".."
        if ((!strncmp(d->d_name, ".", 1)) || (!strncmp(d->d_name, "..", 2)))
            continue;

        string src = fileDir + '/' + d->d_name;
        stat(src.c_str(), &st);
        if (S_ISDIR(st.st_mode))
            writeDirVerifyInfo(src);
        else
            writeVerifyInfo(src);
    }
    closedir(dir);
}

string getMD5(string x)
{
    for (int i = 0; i < x.length(); i++)
    {
        if (x[i] != ' ')
            continue;
        else
            return x.substr(i + 1);
    }
}

string getPath(string x)
{
    for (int i = 0; i < x.length(); i++)
    {
        if (x[i] != ' ')
            continue;
        else
            return x.substr(0, i);
    }
}

bool Verify::VerifyDir(string file)
{
    DIR *dir;
    struct dirent *d;
    struct stat st;
    dir = opendir(file.c_str());
    while ((d = readdir(dir)) != nullptr)
    {
        //ignore "." and ".."
        if ((!strncmp(d->d_name, ".", 1)) || (!strncmp(d->d_name, "..", 2)))
            continue;

        string src = file + '/' + d->d_name;
        stat(src.c_str(), &st);
        if (S_ISDIR(st.st_mode))
        {
            if (!VerifyDir(src))
            {
                cout<<"failed, dir is:"<<src<<endl;
                return false;
            }
        }
        else
        {
            if(S_ISFIFO(st.st_mode))
            {
                return true;
            }
            if(!VerifySingleFile(src))
            {
                cout<<"failed, src is:"<<src<<endl;
                return false;
            }
        }
    }
    closedir(dir);
    return true;
}

void Verify::readVerifyFile(string file)
{
    char buf[300];
    FILE *p = fopen(file.c_str(), "r");

    //add filename and md5 into map to verify
    while (fgets(buf, 300, p) != nullptr)
    {
        string x(buf);
        string md = getMD5(x);
        string path = getPath(x);
        pair<string, string>a=pair<string, string>(path, md);
        fileMD5.insert(a);
    }
}

bool Verify::VerifySingleFile(string file)
{
    char* p=realpath(file.c_str(),nullptr);
    struct stat st;
    string path=p;
    stat(path.c_str(),&st);
    auto i=fileMD5.find(path);
    if(i!=fileMD5.end())
    {
        string md=i->second;
            if(S_ISFIFO(st.st_mode))
            {
                if(md=="12345")
                    return true;
            }
        md=md.substr(0,md.length()-1);
        string mdCurrent=MD5.Encode(path,true);
        if(md!=mdCurrent)
            return false;
        else return true;
    }
    else
    {
        return false;
    }
}

bool Verify::verify(string verifyFile,string file)
{
    fileMD5.clear();
    readVerifyFile(verifyFile);
    return VerifyDir(file);
}

Verify::Verify()
{
    fd = open("/home/wzy/Verify.ver", O_CREAT | O_RDWR|O_TRUNC,0777);
    umask(0000);
    chmod("./Verify.ver", 0777);
}
