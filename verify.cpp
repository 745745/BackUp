#include "./verify.h"

void Verify::writeVerifyInfo(string verify)
{
    char *path;
    path = realpath(verify.c_str(), nullptr);
    string p(path);
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
                return false;
            }
        }
        else
        {
            if(VerifySingleFile(src))
                return false;
        }
    }
    return true;
}

void Verify::readVerifyFile(string file)
{
    char buf[80];
    FILE *p = fopen(file.c_str(), "r");

    //add filename and md5 into map to verify
    while (fgets(buf, 100, p) != nullptr)
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
    auto i=fileMD5.find(file);
    if(i!=fileMD5.end())
    {
        string md=i->second;
        string mdCurrent=MD5.Encode(file,true);
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
    fd = open("./Verify.ver", O_CREAT | O_RDWR);
    chmod("./Verify.ver", 0777);
}