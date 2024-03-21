#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>

#include <sys/ipc.h>

#include <sys/shm.h>

#define BUFFER_SIZE 55
#define MAX_LINE_LENGTH 100

int main()
{

    struct shmid_ds b;
    key_t k; // key to identify shared memory segment
    // Generate a key for the shared memory segment
    if ((k = ftok("hotelmanager.c", 'A')) == -1)
    {
        perror("Error in ftok\n");
        return 1;
    }
    int sid;   // shared memory segment identifier
    int *sPtr; // Pointer to the shared memory segment

    sid = shmget(k, BUFFER_SIZE, 0644 | IPC_CREAT); // IPC_PRIVATE as 1st arg.
    if (sid == -1)
    {
        perror("Error in shmget in creating/ accessing shared memory\n");
        return 1;
    }
    // Attach the shared memory segment to the parent process
    sPtr = shmat(sid, NULL, 0);
    if (sPtr == (void *)-1)
    {
        perror("Error in shmPtr in attaching the memory segment\n");
        return 1;
    }

    char at;
    while (1)
    {

        printf("Do you want to close the hotel? Enter Y for Yes and N for No.");
        sPtr[0] = 0;

        scanf("%c", &at);

        if (at == 'y' || at == 'Y')
        {
            sPtr[0] = 1;
            break;
        }
    }
    return 0;
}
