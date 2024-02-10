#include "kernel/types.h"
#include "user/user.h"

int
main( int argc, char const * argv[]) {
    if (argc != 2) {
        printf("sleep should have one parameter\n");
        exit(0);
    }
    int arg = atoi(argv[1]);
    sleep(arg);
    exit(0);
}