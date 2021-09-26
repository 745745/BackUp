#include "./pack.h"
Verify *ver=new Verify();
extern int getFileMode(struct stat *st);
//link is the src link file. linkName is field been set
//when hardLink=true, link is the link path
void setLinkName(bool hardLink, int mode, char *link, char *linkName)
{
    if (hardLink)
    {
        memcpy(linkName, link, strlen(link));
        return;
    }
    //soft link
    if (S_ISLNK(mode))
    {
        char buf[100];
        int size = readlink(link, buf, 100);
        buf[size] = '\0';
        memcpy(linkName, buf, size + 1);
    }
    else //not a link
        memset(linkName, 0, 100);
}

string getLinkName(char *link)
{
    return link;
}

void setTime(char *time, struct stat *st)
{
    long actime = st->st_atim.tv_sec;
    long modtime = st->st_mtim.tv_sec;
    memcpy(time, &modtime, 8);
    memcpy(time + 8, &actime, 8);
}

long getAcTime(char *time)
{
    long actime = 0;
    memcpy(&actime, time, 8);
    return actime;
}

long getModTime(char *time)
{
    long modtime = 0;
    memcpy(&modtime, time, 8);
    return modtime;
}

void setMode(char *mode, struct stat *st)
{
    int mod = getFileMode(st);
    memcpy(mode, &mod, 4);
}

int getMode(char *mode)
{
    int mod;
    memcpy(&mod, mode, 4);
    return mod;
}

void setOwner(char *id, struct stat *st)
{
    int uid = st->st_uid;
    int gid = st->st_gid;
    memcpy(id, &uid, 4);
    memcpy(id + 4, &gid, 4);
}

int getUid(char *id)
{
    int uid = 0;
    memcpy(&uid,id, 4);
    return uid;
}

int getGid(char *id)
{
    int gid = 0;
    memcpy(&gid,id, 4);
    return gid;
}

void setSize(char *size, long len)
{
    memcpy(size, &len, 8);
}

long getSize(char *size)
{
    long s=0;
    memcpy(&s,size,8);
    return s;
}

void setMagic(char *mag)
{
    memcpy(mag, magic, 8);
}

void setType(bool hardlink, char *type, struct stat *st)
{

    char typ;
    if (hardlink)
    {
        typ = HARDLNK;
        memcpy(type, &typ, 1);
        return;
    }
    if (S_ISREG(st->st_mode))
        typ = REGULAR;
    else if (S_ISLNK(st->st_mode))
        typ = SymLNK;
    else if (S_ISDIR(st->st_mode))
        typ = DIRENT;
    else if (S_ISFIFO(st->st_mode))
        typ = FIFO;
    memcpy(type, &typ, 1);
}

string packFile::findInode(int inode)
{
    string a("");
    auto i = inode_name.find(inode);
    if (i != inode_name.end())
        return i->second;
    else
        return a;
}

string deletePrefix(string path)
{
    string newPath = path;
    while (newPath.substr(0, 3) == "../")
    {
        newPath = newPath.substr(3);
    }
    return newPath;
}

void packFile::addInode(int inode, string name)
{
    inode_name.insert(pair<int, string>(inode, name));
}

char *packFile::setHead(string src)
{
    struct stat statbuf;
    stat(src.c_str(), &statbuf);
    char *block = new char[512];
    memset(block, 0, 512);
    Record::head *p = (Record::head *)block;
    memcpy(p->name, src.c_str(), src.length()); //set name

    string a = findInode(statbuf.st_ino);
    bool hardlink = a.length();
    if (hardlink)
        setLinkName(true, statbuf.st_mode, (char *)a.c_str(), p->linkName);
    else
        setLinkName(false, statbuf.st_mode, (char *)src.c_str(), p->linkName);
    setTime(p->modTime, &statbuf);
    setMode(p->mode, &statbuf);
    setOwner(p->uid, &statbuf);
    setSize(p->size, statbuf.st_size);
    setMagic(p->magic);
    setType(hardlink, p->type, &statbuf);
    if (!hardlink)
        addInode(statbuf.st_ino, src); //add inode & name to map
    return block;
}

void packFile::readFile(string src)
{
    char *head = setHead(src);
    blocks.push_back(head);
    int size = 0;
    memcpy(&size, ((Record::head *)head)->size, 4);

    char type;
    memcpy(&type, ((Record::head *)head)->type, 1);
    if (type == SymLNK || type == FIFO || type == HARDLNK || type == DIRENT) //symlink do not have to copy content
        return;
    int fd = open(src.c_str(), O_RDONLY);
    int readsize;
    char buf[512];
    while ((readsize = read(fd, buf, 512)) >= 0)
    {
        if (readsize == 0)
            break;
        char *content = new char[512];
        memset(content, 0, 512);
        memcpy(content, buf, readsize);
        blocks.push_back(content);
    }
    close(fd);
    return;
}

void packFile::addFile(string src)
{
    file.push_back(src);
}

int packFile::addFileFromDir(string dirPath)
{
    DIR *dir;
    struct dirent *d;
    struct stat st;

    if ((dir = opendir(dirPath.c_str())) == nullptr)
    {
        cout << "open dir error,path is:" << string(dirPath) << endl;
        return -1;
    }
    addFile(dirPath);

    while ((d = readdir(dir)) != nullptr)
    {
        //ignore "." and ".."
        if ((!strncmp(d->d_name, ".", 1)) || (!strncmp(d->d_name, "..", 2)))
            continue;

        string path = dirPath + "/" + d->d_name;
        if (d->d_type == DT_DIR)
        {
            addFileFromDir(path);
        }
        else
            addFile(path);
    }
    return 1;
}

void packFile::loadAllFile()
{
    for (auto i : file)
    {
        readFile(i);

    }
}

void packFile::saveAsFile(string dest)
{
    string fileName = dest + "/" + "packFile.wzy";
    umask(0000);
    remove(fileName.c_str());
    int fd = open(fileName.c_str(), O_WRONLY|O_CREAT|O_TRUNC, 0777);
    perror("open");
    for (int i = 0; i < blocks.size(); i++)
    {
        write(fd, blocks[i], 512);
    }
    close(fd);
}

bool checkHead(char *buf)
{
    Record::head *p = (Record::head *)buf;
    if (memcmp(p->magic, magic, 8) == 0)
    {
        return true;
    }
    else
        return false;
}



string getRealPath(string &dest,string src,string dir)
{
    if(src==dir)
    {
        for(int i=dir.length();;i--)
        {
            if(src[i]=='/')
            {
                string x=src.substr(i);
                dest=dest+x;
                return dest;
            }
        }
    }
    int pos=src.find(dir);
    if(pos==src.npos)
    {
        return src;
    }
    string p=src.substr(pos+dir.length()+1);
    return dest+"/"+p;
}

//dest must exist
void packFile::unpack(string dest, string packSrc)
{
    if (access(dest.c_str(), F_OK))
    {
        if (mkdir(dest.c_str(), 0777))
        {
            perror("");
            cout << "\nmake dest dir failed\n";
            return;
        }
    }
    char *destPath = realpath(dest.c_str(), NULL); //change the relative path to absolute path
    dest = destPath;
    free(destPath);
    char buf[512];
    int fd = open(packSrc.c_str(), O_RDONLY);
    int fdCurrent;
    int readsize = 0;
    long size = 0;

    string dir="";
    while (readsize = read(fd, buf, 512))
    {
        if (checkHead(buf)) //header
        {
            Record::head *p = (Record::head *)buf;
            if(dir.length()==0)
                dir=p->name;
            string qqq=p->name;
            string name = getRealPath(dest,p->name,dir);
            if (access(name.c_str(), F_OK))
            {
                if (*p->type == DIRENT)
                {
                    if (mkdir(name.c_str(), getMode(p->mode)))
                    {
                        perror("");
                        cout << "\nmake dest dir failed\n";
                        return;
                    }
                }
                else if(*p->type==REGULAR)
                {
                    //set Mode
                    fdCurrent = open(name.c_str(), O_RDWR | O_CREAT, getMode(p->mode));
                }
                if(*p->type==HARDLNK)
                {
                    link(p->linkName,name.c_str());
                }
                else if(*p->type==SymLNK)
                {
                    symlink(p->linkName,name.c_str());
                }
                else if(*p->type==FIFO)
                {
                    mkfifo(name.c_str(),0777);
                }
                //set owner
                chown(name.c_str(), getUid(p->uid), getGid(p->gid));

                //set time
                struct utimbuf timebuf;
                timebuf.actime = getAcTime(p->actTime);
                timebuf.modtime = getModTime(p->modTime);
                utime(name.c_str(), &timebuf);

                size = getSize(p->size);

            }
        }
        else //content
        {
            if (size >= 512)
            {
                size -= 512;
                write(fdCurrent, buf, readsize);
                close(fdCurrent);
            }
            else
            {
                write(fdCurrent, buf, size);
            }
        }
    }
}

