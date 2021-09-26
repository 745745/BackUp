#include "./copy.h"
using namespace std;
extern Verify *ver;

map<int, string> inode_name; //used for hard link. If an inode has already been in this map,use link() instead of copy again

void addInodeToMap(int inode, string fileName)
{
    inode_name.insert(pair<int, string>(inode, fileName));
}

string findInode(int inode)
{
    string a = "";
    auto t = inode_name.find(inode);
    if (t != inode_name.end())
        return t->second;
    else
        return a;
}

int getInodeNumber(struct stat *st)
{
    return st->st_ino;
}

int getFileMode(struct stat *st)
{
    int user = 0;  //user mode
    int group = 0; //group mode
    int other = 0; //other mode
    if (st->st_mode & S_IREAD)
        user += 4;
    if (st->st_mode & S_IWRITE)
        user += 2;
    if (st->st_mode & S_IEXEC)
        user += 1;

    if (st->st_mode & S_IRGRP)
        group += 4;
    if (st->st_mode & S_IWGRP)
        group += 2;
    if (st->st_mode & S_IXGRP)
        group += 1;

    if (st->st_mode & S_IROTH)
        other += 4;
    if (st->st_mode & S_IWOTH)
        other += 2;
    if (st->st_mode & S_IXOTH)
        other += 1;

    return user * 64 + group * 8 + other; //octal
}

int setFileMode(char *path, int mode)
{
    if (chmod(path, mode) == -1)
    {
        return -1;
    }
    return 1;
}

int setFileOwner(char *dest, struct stat *st)
{
    //struct passwd *pw = getpwuid(st->st_uid);
    //struct group *gr = getgrgid(st->st_gid);

    if (chown(dest, st->st_uid, st->st_gid) == -1)
    {
        return -1;
    }
    return 1;
}

int setFileTime(char *dest, struct stat *st)
{
    struct utimbuf timebuf;
    timebuf.actime = st->st_atim.tv_sec;
    timebuf.modtime = st->st_mtim.tv_sec;
    if (utime(dest, &timebuf) < 0)
    {
        return -1;
    }
    return 1;
}

int setFileAttribute(char *dest, struct stat *st)
{
    if (setFileMode(dest, getFileMode(st)) == -1)
    {
        perror("Error mode:");
        return -1;
    }

    if (setFileOwner(dest, st) == -1)
    {
        cout << "change owner/group false\n";
        return -1;
    }

    if (setFileTime(dest, st) == -1)
    {
        cout << "set time failed\n";
        return -1;
    }
    return 1;
}

string getFileName(string src)
{

    int len = 0;
    for (int i = src.length(); src[i] != '/'; i--)
    {
        len++;
    }

    return src.substr(src.length() - len);
}

int copyFile(string dest, string src)
{
    struct stat statbuf;
    stat(src.c_str(), &statbuf);
    int size = statbuf.st_size;
    int d, s;

    //s = open(src.c_str(), O_RDONLY | O_CREAT, 0777);
    s = open(src.c_str(), O_RDONLY);
    if (access(dest.c_str(), F_OK)) //dest file do not exist, create one and copy the content from src file
    {
        d = open(dest.c_str(), O_WRONLY | O_CREAT);
        if (s <= 0)
        {
            cout << "can not open src file,path is:" << string(src) << endl;
            return -1;
        }
        if (d <= 0)
        {
            cout << "can not open dest file,path is:" << string(dest) << endl;
            return -1;
        }

        int inodeNum=getInodeNumber(&statbuf);
        string result = findInode(inodeNum);
        if (result.size() == 0)//src file has no hard link
        {
            addInodeToMap(inodeNum,dest);
            char *buf = new char[size];
            memset(buf, 0, size);
            int ret = read(s, buf, size);
            if (ret < 0)
            {
                perror("read:");
                cout << "read error" << endl;
                return -1;
            }
            write(d, buf, size);
        }
        else //src file has hard links, use link instead of copy
        {
            remove(dest.c_str());   
            link(result.c_str(),dest.c_str());
        }
        setFileAttribute((char *)dest.c_str(), &statbuf);
    }
    close(d);
    close(s);
    return 1;
}

int copyDir(string dest, string src)
{

    if (access(dest.c_str(), F_OK)) //dest dir is not exist
    {
        struct stat statbuf;
        stat(src.c_str(), &statbuf);
        if (mkdir(dest.c_str(), getFileMode(&statbuf)))
        {
            cout << "make dest dir failed\n";
            return -1;
        }
    }

    DIR *dir;
    struct dirent *d;
    struct stat st;
    if (stat(src.c_str(), &st) < 0 || !S_ISDIR(st.st_mode))
    {
        cout << "wrong src path:" << string(src) << endl;
        return -1;
    }

    if ((dir = opendir(src.c_str())) == nullptr)
    {
        cout << "open dir error,path is:" << string(src) << endl;
        return -1;
    }

    while ((d = readdir(dir)) != nullptr)
    {
        //ignore "." and ".."
        if ((!strncmp(d->d_name, ".", 1)) || (!strncmp(d->d_name, "..", 2)))
            continue;

        string destPath = dest + "/" + d->d_name;
        string srcPath = src + "/" + d->d_name;
        if (copy(false, destPath, srcPath) == -1)
        {
            cout << "copy File failed,src file is:" << srcPath << " dest file is:" << destPath << endl;
            return -1;
        }
    }
    closedir(dir);
    return 1;
}

int copyLink(string dest, string src)
{
    char buf[128];
    int size = readlink(src.c_str(), buf, 128);
    buf[size] = '\0';
    symlink(buf, dest.c_str());
    return 1;
}

int copyPipe(string dest, string src)
{
    struct stat st;
    stat(src.c_str(), &st);
    mkfifo(dest.c_str(), getFileMode(&st));
    return 1;
}

int copy(bool init, string dest, string src)
{
    struct stat st;
    int ret = lstat(src.c_str(), &st);
    if (ret < 0)
    {
        cout << "get src stat failed\n";
        return -1;
    }

    if (init && access(dest.c_str(), F_OK)) //dest dir is not exist
    {
        struct stat statbuf;
        if (mkdir(dest.c_str(), 0777))
        {
            cout << "make dest dir failed\n";
            return -1;
        }
    }

    if (init)
        dest = dest + getFileName(src);

    if (S_ISLNK(st.st_mode))
        copyLink(dest, src);
    else if (S_ISREG(st.st_mode))
    {
        copyFile(dest, src);
    }
    else if (S_ISDIR(st.st_mode))
    {
        copyDir(dest, src);
    }
    else if (S_ISFIFO(st.st_mode))
        copyPipe(dest, src);

    return 1;
}

vector<string> copySelectFile(string dest, vector<string> fileName)
{
    vector<string> failedFile;
    for (int i = 0; i < fileName.size(); i++)
    {
        string src = fileName[i];
        copy(true, dest, src);
    }
    return failedFile;
}
