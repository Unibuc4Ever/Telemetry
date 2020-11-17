#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

int main()
{
    printf("Am I a broadcaster? (1 -> Yes, 0 -> No)\n $ ");
    fflush(stdout);

    int x;
    scanf("%d", &x);

    int shm_fd = shm_open("MySharedMem", O_CREAT|O_RDWR, S_IRUSR|S_IWUSR);
    if (shm_fd < 0) {
        printf("Error: %d\n", shm_fd);
        return 0;
    }

    if (ftruncate(shm_fd, 1000) == -1) {
        printf("Nope!");
        return 0;
    }

    int* ptr = (int*)mmap(0, 1000, PROT_READ|PROT_WRITE, MAP_SHARED, shm_fd, 0);

    if (x == 1) {
        int i = 0;
        for (i = 1; i; i++) {
            ptr[0] = i;
            printf("Broadcasting %d...\n", i);
            sleep(1);
        }
    }

    else {
        while (1) {
            printf("Value of memory: %d\n", ptr[0]);
            sleep(1);
        }
    }


    return 0;
}