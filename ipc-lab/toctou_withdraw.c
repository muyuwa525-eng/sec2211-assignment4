#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    int amount = atoi(argv[1]);

    FILE *f = fopen("balance.txt", "r");
    int balance;
    fscanf(f, "%d", &balance);
    fclose(f);
    printf("[pid %d] CHECK: balance = %d, want to withdraw %d\n", getpid(), balance, amount);

    if (balance >= amount) {
        printf("[pid %d] Check passed. Sleeping 2s (the race window)...\n", getpid());
        sleep(2);

        FILE *f2 = fopen("balance.txt", "w");
        fprintf(f2, "%d\n", balance - amount);
        fclose(f2);
        printf("[pid %d] USE: withdrew %d, wrote new balance = %d\n",
               getpid(), amount, balance - amount);
    } else {
        printf("[pid %d] Insufficient funds, aborting.\n", getpid());
    }
    return 0;
}
