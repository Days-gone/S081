#include "kernel/types.h"
#include "user/user.h"

#define NUMBER 34
#define BUFSIZE sizeof(int) * NUMBER
#define READEND 0
#define WRITEEND 1
#define STDIN 0
#define STDOUT 1

int buf[NUMBER];

int is_prime(int x)
{
    if (x < 2)
        return 0;
    for (int i = 2; i <= x / i; i++)
    {
        if (x % i == 0)
            return 0;
    }
    return 1;
}

void init()
{
    // let buf is 2..35
    // let element be -1 if prime
    for (int i = 0; i < 34; i++)
    {
        buf[i] = i + 2;
    }
}

void cps_prime(int read_fd)
{
    int buffer[NUMBER];
    read(read_fd, buffer, BUFSIZE);
    close(read_fd);
    int idx = 0;
    while (is_prime(buffer[idx]) == 0 && idx < NUMBER)
        idx++;
    if (idx == NUMBER)
        exit(0);
    int my_p[2];
    pipe(my_p);
    int my_pid = fork();
    if (my_pid < 0)
    {
        printf("[ERROR] cannot fork\n");
        exit(0);
    }
    if (my_pid == 0)
    {
        cps_prime(my_p[READEND]);
        exit(0);
    }
    else
    {
        printf("prime %d\n", buffer[idx]);
        for (int i = idx + 1; i < NUMBER; i++)
        {
            if ((buffer[i] % buffer[idx]) == 0)
                buffer[i] = -1;
        }
        buffer[idx] = -1;
        write(my_p[WRITEEND], buffer, BUFSIZE);
        wait((int *)0);
        exit(0);
    }
}

int main(int argc, char const *argv[])
{
    int p[2];
    pipe(p);
    init();
    int pid = fork();
    if (pid == 0)
    {
        cps_prime(p[READEND]);
        exit(0);
    }
    else
    {
        write(p[WRITEEND], buf, BUFSIZE);
        close(p[WRITEEND]);
        wait((int *)0);
    }
    exit(0);
}