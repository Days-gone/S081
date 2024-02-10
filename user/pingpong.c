#include "kernel/types.h"
#include "user/user.h"

char buf[512];


int
main(int argc, char const * argv[])
{
    int p[2];
    pipe(p);
    int pid = fork();
    if (pid == 0){
        read(p[0], buf, 1);
        printf("%d: received ping\n", getpid());
        write(p[1], "A", 1);
        exit(0);
    }
    else{
        write(p[1], "A", 1);
        wait((int*) 0 );
        read(p[0], buf, 1);
        printf("%d: received pong\n", getpid());
        exit(0);
    }
    exit(0);
}