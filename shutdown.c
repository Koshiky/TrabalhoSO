#include "utilizados.h"

int main(){
    int idshm, *pid;

    shmid = shmget(KPAI, sizeof(int), IPC_CREAT|0600); //Owner can read and write
    pid = (int *) shmat(idshm, NULL, 0);

    kill(*pid, SIGUSR1);

    return 0;
}
