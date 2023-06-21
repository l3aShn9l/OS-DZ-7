#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <time.h>

#define SHARED_MEM_SIZE sizeof(int)

const char* shar_object = "posix-shar";

int main(int argc, char *argv[]) {
int loop_count;

if (argc != 2) {
printf("Incorrect args\n");
exit(1);
} else {
loop_count = atoi(argv[1]);
}

int shm_id = shm_open(shar_object, O_CREAT | O_RDWR, 0666);
if (shm_id == -1) {
perror("shm_open");
exit(1);
} else {
printf("Object is open: name = %s, id = 0x%x\n", shar_object, shm_id);
}

if (ftruncate(shm_id, SHARED_MEM_SIZE) == -1) {
perror("ftruncate");
exit(1);
}

int *ptr = mmap(0, SHARED_MEM_SIZE, PROT_WRITE|PROT_READ, MAP_SHARED, shm_id, 0);
if (ptr == (int*)-1) {
perror("mmap");
exit(1);
}

srand(time(NULL));

int i = 0;
*ptr = -1;
while (i < loop_count && *ptr != -3) {
if (*ptr == -3) {
break;
}
int random_num = rand() % 1000;
while (*ptr != -1 && *ptr != -3) {
usleep(1000);
}
if (*ptr == -3) {
break;
}
*ptr = random_num;
printf("Generated number: %d\n", random_num);
i++;
}

*ptr = -2;

if (shm_unlink(shar_object) == -1) {
perror("shm_unlink");
exit(1);
}

exit(0);
}
