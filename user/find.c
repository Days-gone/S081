#include "kernel/types.h"
#include "kernel/fs.h"
#include "user/user.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"

#define STDERROR 2
#define BUFSIZE 512

int post_find(char *parent, char const *child)
{
    int par_len = strlen(parent);
    int last_slash = par_len - 1;
    while (parent[last_slash] != '/')
        last_slash--;
    if (strcmp(parent + last_slash + 1, child) == 0)
        return 1;
    return 0;
}

void find(char *path, char *target)
{
    char buf[BUFSIZE], *p;
    struct dirent de;
    struct stat st;
    int fd;
    if ((fd = open(path, O_RDONLY)) < 0)
    {
        fprintf(STDERROR, "find: cannot open %s\n", path);
        return;
    }
    if (fstat(fd, &st) < 0)
    {
        fprintf(STDERROR, "cannot stat %s\n", path);
        close(fd);
        return;
    }
    if (st.type != T_DIR)
    {
        printf("find should have a dir para\n");
        close(fd);
        return;
    }
    if (strlen(path) + 1 + DIRSIZ + 1 > sizeof(buf))
    {
        printf("dir para is too long\n");
        close(fd);
        return;
    }
    strcpy(buf, path);
    p = buf + strlen(buf);
    *(p++) = '/';
    // set the p to the path/ slash position and
    // memmove just override the value in the pointers' positions
    while (read(fd, &de, sizeof(de)) == sizeof(de))
    {
        if (de.inum == 0)
            continue;
        memmove(p, de.name, DIRSIZ);
        p[DIRSIZ] = 0;
        if (stat(buf, &st) < 0)
        {
            printf("cannot stat %s\n", buf);
            continue;
        }
        if (st.type == 1)
        {
            if (post_find(buf, ".") == 1 || post_find(buf, "..") == 1)
            {
                continue;
            }
            find(buf, target);
        }
        if (post_find(buf, target) == 1)
            printf("%s\n", buf);
    }
    close(fd);
}

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        printf("find should have 2 elemets\n");
        exit(0);
    }
    if (strcmp(argv[2], ".") == 0 || strcmp(argv[2], "..") == 0)
    {
        printf("find could not have . or .. as target\n");
        exit(0);
    }
    find(argv[1], argv[2]);
    exit(0);
}