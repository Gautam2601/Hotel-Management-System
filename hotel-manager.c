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

    int totalTable;

    printf("Enter Total number of tables : ");

    scanf("%d", &totalTable);
    if (totalTable == 0)
    {
        return 0;
    }
    int TotalEarning = 0;

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
    for (int i = 1; i <= totalTable; i++)
    {

        struct shmid_ds bu;
        key_t key; // key to identify shared memory segment

        // Generate a key for the shared memory segment
        if ((key = ftok("waiter.c", i)) == -1)
        {
            perror("Error in ftok\n");
            return 1;
        }
        int shid;   // shared memory segment identifier
        int *shPtr; // Pointer to the shared memory segment

        shid = shmget(key, BUFFER_SIZE, 0644 | IPC_CREAT); // IPC_PRIVATE as 1st arg.
        if (shid == -1)
        {
            perror("Error in shmget in creating/ accessing shared memory\n");
            return 1;
        }
        // Attach the shared memory segment to the parent process
        shPtr = shmat(shid, NULL, 0);
        if (shPtr == (void *)-1)
        {
            perror("Error in shmPtr in attaching the memory segment\n");
            return 1;
        }
        while (shPtr[0] == 0)
        {
            sleep(1);
        }
        TotalEarning += shPtr[0];
        // Creating Earnings File
        char *filename = "earning.txt";
        if(i==1){
        FILE *fp = fopen(filename, "w");

        if (fp == NULL)
        {
            printf("Error opening the file %s", filename);
            return -1;
        }

        fprintf(fp, "Earning from table %d is ", i);
        fprintf(fp, "%d INR", shPtr[0]);
        fprintf(fp, "\n");

        fclose(fp);
        }
        else{
            FILE *fp = fopen(filename, "a");

        if (fp == NULL)
        {
            printf("Error opening the file %s", filename);
            return -1;
        }

        fprintf(fp, "Earning from table %d is ", i);
        fprintf(fp, "%d INR", shPtr[0]);
        fprintf(fp, "\n");

        fclose(fp);
        }

        printf("Earning from Table %d : %d INR\n",i,shPtr[0]);

        if (shmctl(shid, IPC_RMID, 0) == -1)
        {
            perror("Error in shmctl\n");
            return 1;
        }
        
    }

    char *filename = "earning.txt";
    FILE *fp = fopen(filename, "a");
    if (fp == NULL)
    {
        printf("Error opening the file %s", filename);
        return -1;
    }
     fprintf(fp, "Total hotel earning is %d INR\n", TotalEarning);

     fprintf(fp, "\n");

    double waiterEarning = TotalEarning * (0.4);
    double profit = TotalEarning-waiterEarning;
    fprintf(fp, "Waiter's earning is: %f INR\n", waiterEarning);
    fprintf(fp, "Total Profit: %f INR\n", profit);
    
    printf("Total hotel earning is %d INR\n",TotalEarning);
    printf("Waiter's earning is: %f INR\n",waiterEarning);
    printf("Total Profit: %f INR\n",profit);


    while (sPtr[0] != 1)
    {
        sleep(1);
    }
    printf("Thank you for visiting the Hotel!\n");
    return 0;
}