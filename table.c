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
#define READ_END 0
#define WRITE_END 1
#define MAX_LINE_LENGTH 100

int *shmPtr;

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

    // Open the file in read mode
    file = fopen("menu.txt", "r");
    // Check if file opened successfully
    if (file == NULL)
    {
        printf("Error opening file.\n");
        return 1;
    }
    int p = 0;
    // Reading lines until end of file
    while (fgets(line, sizeof(line), file) != NULL)
    {
        if (sscanf(line, "%*d. %[^0-9] %d INR", itemName, &price) == 2)
        {
            sumArr[p] = price;
            p++;
        }
    }

    int finalBillAmount = 0; // final bill amount

    for (int i = 0; i < ipArrSize; i++)
    {
        finalBillAmount += sumArr[shmPtr[i] - 1];
    }

    printf("The bill total is: ");

    printf("%d", finalBillAmount);
    // Close the file
    fclose(file);
    return finalBillAmount;
}

int sharedMemory(int *and, int tableNumber)
{

    struct shmid_ds buf;
    key_t key;

    if ((key = ftok("table.c", tableNumber)) == -1)
    {
        perror("Error in ftok\n");
        return 1;
    }

    int shmid; // shared memory segment identifier

    shmid = shmget(key, BUFFER_SIZE, 0644 | IPC_CREAT); // IPC_PRIVATE as 1st arg.

    if (shmid == -1)
    {
        perror("Error in shmget in creating/ accessing shared memory\n");
        return 1;
    }
    // Attach the shared memory segment to the parent process
    shmPtr = shmat(shmid, NULL, 0);

    if (shmPtr == (void *)-1)
    {
        perror("Error in shmPtr in attaching the memory segment\n");
        return 1;
    }

    for (int i = 0; i < BUFFER_SIZE; i++)
    {
        shmPtr[i] = and[i];
    }
}

int creatMenu(int numberOfCustomers, int *and, int tableNumber)
{
    int write_msg[BUFFER_SIZE];
    int read_msg[BUFFER_SIZE];
    int fd[2];

    int m[1];
    int ml[1];
    ml[0] = 0;

    pid_t pid;

    int x = 0;

    for (int i = 0; i < BUFFER_SIZE; i++)
    {
        and[i] = -2;
    }
    for (int i = 0; i < numberOfCustomers; i++)
    {

        if (pipe(fd) == -1)
        {
            fprintf(stderr, "pipe failed");
            return 1;
        }

        pid = fork();

        if (pid < 0)
        {
            fprintf(stderr, "Fork Failed");
            return 1;
        }

        if (pid > 0)
        {

            sleep(3);

            close(fd[WRITE_END]);

            read(fd[READ_END], read_msg, BUFFER_SIZE);

            int f = ml[0];

            read(fd[READ_END], ml, 1);

            int s = ml[0];

            ml[0] = ml[0] + f;

            int ft = 0;
            if (i == 0)
            {
                s = 0;
            }

            for (int l = f; l < ml[0]; l++)
            {
                and[l] = read_msg[ft++];
            }

            close(fd[READ_END]);
        }
        else
        {

            close(fd[READ_END]);

            char neg = -1;
            printf("Enter the serial number(s) of the item(s) to order from the menu. Enter -1 when done:");

            while (x < BUFFER_SIZE)
            {
                scanf("%d", &write_msg[x]);
                if (write_msg[x] == -1)
                {
                    break;
                }
                x++;
            }

            m[0] = x;

            write(fd[WRITE_END], write_msg, BUFFER_SIZE);

            write(fd[WRITE_END], m, 1);

            close(fd[WRITE_END]);

            while (shmPtr[62] != 1)
            {
                sleep(1);
            }
            

            exit(0);
        }
    }
    sharedMemory(and, tableNumber);
    shmPtr[61] = 2;

    return 0;
}
void readMenu()
{

    FILE *fp;
    char ch;

    fp = fopen("menu.txt", "r");
    while (ch != EOF)
    {
        ch = fgetc(fp);
        if (ch == EOF)
            break;
        printf("%c", ch);
    }

    fclose(fp);
}

int main()
{
    printf("Enter Table Number: ");
    int tableNumber, numberOfCustomers;
    scanf("%d", &tableNumber);
    printf("Enter Number Of Customers: ");
    scanf("%d", &numberOfCustomers);

    readMenu();

    int and[BUFFER_SIZE];

    int newCust = 0;

    sharedMemory(and, tableNumber);

    while (newCust != -1)
    {

        if (newCust != 0)
        {

            printf("Enter Number Of Customers: ");
            scanf("%d", &numberOfCustomers);

            readMenu();
        }
        creatMenu(numberOfCustomers, and, tableNumber);

        shmPtr[57] = 0;

        shmPtr[55] = 0;
        shmPtr[56] = -5;
        while (shmPtr[56] != -6)
        {
            sleep(1);
        }

        while (shmPtr[55] == -1)
        {
            creatMenu(numberOfCustomers, and, tableNumber);
            shmPtr[57] = 0;
            shmPtr[56] = -5;
            while (shmPtr[56] != -6)
            {
                sleep(1);
            }
        }

        printf("Total Bill Amount is %d", shmPtr[59]);
        printf("\n");
        shmPtr[57] = -2;
        printf("Are new customers coming ");
        scanf("%d", &newCust);
        shmPtr[57] = newCust;
    }
    return 0;
}