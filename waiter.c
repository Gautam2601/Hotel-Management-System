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

int TotalBill(int *shmPtr)
{

    int ipArrSize = 0; // size of the array received from table

    for (int i = 0; i < BUFFER_SIZE; i++)
    {

        if (shmPtr[i] == -2)
        {
            break;
        }
        ipArrSize++;
    }

    // Reading food item's price from menu file
    FILE *file;
    char line[MAX_LINE_LENGTH]; // Buffer to store each line
    char itemName[MAX_LINE_LENGTH];
    int sumArr[MAX_LINE_LENGTH];
    int price;
    // Opening file
    file = fopen("menu.txt", "r");

    // Check if file is opened successfully
    if (file == NULL)
    {
        printf("Error opening file.\n");
        return 1;
    }
    int p = 0;
    while (fgets(line, sizeof(line), file) != NULL)
    {
        if (sscanf(line, "%*d. %[^0-9] %d INR", itemName, &price) == 2)
        {
            sumArr[p] = price;
            p++;
        }
    }

    int finalBillAmount = 0;

    for (int i = 0; i < ipArrSize; i++)
    {
        finalBillAmount += sumArr[shmPtr[i] - 1];
    }

    fclose(file);
    return finalBillAmount;
}
// Creating Menu.txt
int NOI()
{
    FILE *fp;
    char ch;
    fp = fopen("menu.txt", "r");
    int numberOfLines = 0;
    while (ch != EOF)
    {
        ch = fgetc(fp);
        if (ch == '\n')
        {
            numberOfLines++;
        }
        if (ch == EOF)
            break;
    }
    fclose(fp);

    return numberOfLines;
}

void orderValid(int *shmPtr, int numberOfItems)
{

    shmPtr[55] = 0;

    for (int i = 0; i < BUFFER_SIZE; i++)
    {
        if (shmPtr[i] == -2)
        {
            break;
        }
        if (shmPtr[i] < 1 || shmPtr[i] > numberOfItems)
        {
            shmPtr[55] = -1;
            return;
        }
    }
    shmPtr[56] = -6;
}

int main()
{

    printf("Enter waiter ID: ");
    int waiterNumber;
    struct shmid_ds buf;

    key_t key;
    int shmid;   // shared memory segment identifier
    int *shmPtr; // Pointer to the shared memory segment
    scanf("%d", &waiterNumber);
    // Creating Child Process

    if ((key = ftok("table.c", waiterNumber)) == -1)
    {
        perror("Error in ftok\n");
        return 1;
    }

    shmid = shmget(key, BUFFER_SIZE, 0644 | IPC_CREAT);

    if (shmid == -1)
    {
        perror("Error in shmget in creating/ accessing shared memory\n");
        return 1;
    }
    // Attaching the shared memory segment to the parent process
    shmPtr = shmat(shmid, NULL, 0);

    if (shmPtr == (void *)-1)
    {
        perror("Error in shmPtr in attaching the memory segment\n");
        return 1;
    }

    while (shmPtr[61] != 2)
    {
        sleep(1);
    }
    int numberOfItems = NOI();

    int TotalEarning = 0;
    while (shmPtr[57] != -1)
    {
        shmPtr[62] = 0;

        orderValid(shmPtr, numberOfItems);

        while (shmPtr[55] == -1)
        {
            while (shmPtr[56] != -5)
            {
                sleep(1);
            }

            orderValid(shmPtr, numberOfItems);
            shmPtr[56] = -6;
        }

        int tota = TotalBill(shmPtr);

        TotalEarning = TotalEarning + tota;
        shmPtr[62] = 1;

        printf("Total bill sent : ");
        shmPtr[59] = tota;
        printf("%d\n",tota);

        if (shmPtr[57] == -1)
        {
            break;
        }
        while (shmPtr[57] != -2)
        {
            if (shmPtr[57] == -1)
            {
                break;
            }
            sleep(1);
        }

        while (shmPtr[57] != 0)
        {
            if (shmPtr[57] == -1)
            {
                break;
            }

            sleep(1);
        }
    }

   if (shmctl(shmid, IPC_RMID, 0) == -1) 
    {
        perror("Error in shmctl\n");
        return 1;
    }

    if (shmctl(shmid, IPC_RMID, 0) == -1)
    {
        perror("Error in shmctl\n");
        return 1;
    }

    struct shmid_ds bu;
    key_t ky; // key to identify shared memory segment
    if ((ky = ftok("waiter.c", waiterNumber)) == -1)
    {
        perror("Error in ftok\n");
        return 1;
    }
    int shid;   // shared memory segment identifier
    int *shPtr; // Pointer to the shared memory segment

    shid = shmget(ky, BUFFER_SIZE, 0644 | IPC_CREAT); // IPC_PRIVATE as 1st arg.
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

    shPtr[0] = TotalEarning;


    if (shmdt(shPtr) == -1)
    {
        perror("Error in shmdt in detaching the memory segment\n");
        return 1;
    }

    

    
    
    if (shmctl(shid, IPC_RMID, 0) == -1) 
    {
        perror("Error in shmctl\n");
        return 1;
    }
}