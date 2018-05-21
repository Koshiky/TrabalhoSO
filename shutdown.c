#include "utilizados.h"

int main(){
    int shmid, *pid;

    shmid = shmget(KPAI, sizeof(int), IPC_CREAT|0600); //Owner can read and write
    pid = (int *) shmat(shmid, NULL, 0);

    kill(*pid, SIGUSR1);

    return 0;
}
