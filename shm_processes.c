#include  <stdio.h>
#include  <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/shm.h>
#include <sys/wait.h>

#define NUM_ITERATIONS 25

int getRandom(int min, int max) {
     return rand() % (max - min + 1) + min;
}

void parentProcess(int *shared_memory) {
     for (int i = 0; i < NUM_ITERATIONS; i++) {
          sleep(getRandom(0, 5));

          int account = shared_memory[0];

          while (shared_memory[1] != 0);

          if (account <= 100) {
               int balance = getRandom(0, 100);
               if (balance % 2 == 0) {
                    account += balance;
                    printf("Dear old Dad: Deposits $%d / Balance = $%d\n", balance, account);
               } else {
                    printf("Dear old Dad: Doesn't have any money to give\n");
               }
          } else {
               printf("Dear old Dad: Thinks Student has enough Cash ($%d)\n", account);
          }

          shared_memory[0] = account;
          shared_memory[1] = 1;
     }
}

void childProcess(int *shared_memory) {
     for (int i = 0; i < NUM_ITERATIONS; i++) {
          sleep(getRandom(0, 5));

          int account = shared_memory[0];

          while (shared_memory[1] != 1);

          int balance = getRandom(0, 50);
          printf("Poor Student needs $%d\n", balance);
          if (balance <= account) {
               account -= balance;
               printf("Poor Student: Withdraws $%d / Balance = $%d\n", balance, account);
          } else {
               printf("Poor Student: Not Enough Cash ($%d)\n", account);
          }

          shared_memory[0] = account;
          shared_memory[1] = 0;
     }
}

int main() {
     srand(time(NULL));

     int shm_id = shmget(IPC_PRIVATE, sizeof(int) * 2, IPC_CREAT | 0666);
     if(shm_id < 0) {
          perror("shmget failed");
          exit(1);
     }

     int *shared_memory = (int *) shmat(shm_id, NULL, 0);
     if (shared_memory == (int *)-1) {
          perror("shmat failed");
          exit(1);
     }

     shared_memory[0] = 0;
     shared_memory[1] = 0;

     pid_t pid = fork();

     if (pid < 0) {
          perror("fork failed");
          exit(1);
     }

     if (pid == 0) {
          childProcess(shared_memory);
          exit(0);
     } else {
          parentProcess(shared_memory);

          wait(NULL);

          shmdt(shared_memory);
          shmctl(shm_id, IPC_RMID, NULL);
          printf("Shared memory cleaned up. Exiting program.\n");
          exit(0);
     }
}
