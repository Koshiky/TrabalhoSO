#include  "utilizados.h"

int *nProc;

void shutdown(){
    int i, id, job;
    struct mensagem msg;
    
    if(*nProc > 0){
        printf("Ainda ha processos em execucao ou em espera\n");
        printf("Escalonador esta sendo finalizado\n");
        printf("Processos restantes nao serao executados\n");
    }
    
    //Kill os filhos
    
    //Preparar para print dos processos que foram executados
    id = msgget(KFILA, IPC_CREAT|0600);
    
    //Print dos Jobs
    while(msgrcv(id, &msg, sizeof(struct mensagem), DONE, IPC_NOWAIT) != -1){
        if(msg.exec.job != job) {
            printf("\n");
            job = msg.exec.job;
        }
        
        printf("Job: %d Name: %s Delay: %ds Turnaround: %fs\n", msg.exec.job, msg.exec.name, msg.exec.delay, msg.exec.time);
    }
    
    
    //Destroi memorias compartilhadas e filas de mensagem
    msgctl(id, IPC_RMID, NULL);
    
    id = shmget(KJOB, sizeof(int), IPC_CREAT|0600);
    shmctl(id, IPC_RMID, NULL);
    
    id = shmget(KPAI, sizeof(int), IPC_CREAT|0600);
    shmctl(id, IPC_RMID, NULL);
    
    id = shmget(KQNT, sizeof(int), IPC_CREAT|0600);
    shmctl(id, IPC_RMID, NULL);
    
    kill(getpid(), SIGTERM);
}

int main(){
    int i, j, shmid, *pidP, *idjob, idfila, delay, pid, estado;
    char nome[100];
    struct mensagem msgrec, msgsen;
    
    idfila = msgget(KFILA, IPC_CREAT|0600);
    
    shmid = shmget(KJOB, sizeof(int), IPC_CREAT|0600);
    idjob = (int *) shmat(shmid, NULL, 0);
    *idjob = 0;
    
    shmid = shmget(KPAI, sizeof(int), IPC_CREAT|0600);
    pidP = (int *) shmat(shmid, NULL, 0);
    *pidP = getpid();
    
    shmid = shmget(KQNT, sizeof(int), IPC_CREAT|0600);
    nProc = (int *) shmat(shmid, NULL, 0);
    *nProc = 0;
    /*----------------------------------------------------*/
    
    //Rotina dos filhos
    if((pid = fork()) == 0){
        while (1) {
            for(i=0; i < 3; i++){
                if (msgrcv(idfila, &msgrec, sizeof(struct mensagem), i+1, IPC_NOWAIT) == -1) {
                    waitpid(-1, &estado, WNOHANG); //Caso fila vazia, wait para lidar com processo zombie, passando pelas filas de prioridades
                }
                else {
                    for(j=0;j<msgrec.exec.n;j++) {
                        if((pid = fork()) == 0) {
                            printf("Vou executar\n");
                            execl(msgrec.exec.name, "", 0);
                        }
                        else {
                            printf("Antes wait\n");
                            wait(&estado);
                            printf("Depois wait\n");
                            gettimeofday(&msgrec.exec.fim, NULL);
                            msgrec.exec.time = msgrec.exec.fim.tv_sec - msgrec.exec.ini.tv_sec;
                            msgrec.exec.time += (msgrec.exec.fim.tv_usec - msgrec.exec.ini.tv_usec) / 1000000.0;
                            *nProc = *nProc - 1;
                            msgrec.prioridade = DONE;
                            msgsnd(idfila, &msgrec, sizeof(struct mensagem), 0);
                            
                            printf("Job: %d Delay: %d N: %d Priority: %d Name: %s\n", msgrec.exec.job, msgrec.exec.delay, msgrec.exec.n, msgrec.prioridade, msgrec.exec.name);
                            exit(0);
                        }
                    }
                }
            }
        }
    }
    
    
    
    /*----------------------------------------------------*/
    
    //Rotina do pai
    
    //Espera pelo shutdown
    signal(SIGUSR1, shutdown);
    
    while (1) {
        for(i=0; i < 3; i++){
            if (msgrcv(idfila, &msgrec, sizeof(struct mensagem), i+1, IPC_NOWAIT) == -1) {
                waitpid(-1, &estado, WNOHANG); //Caso fila vazia, wait para lidar com processo zombie, passando pelas filas de prioridades
            }
            else {
                //Envia a mensagem para o outro escalonador apos o delay
                *nProc = *nProc + 1;
                msgsen = msgrec;
                
                if((pid = fork()) == 0){
                    printf("Delay: %d\n",msgsen.exec.delay);
                    
                    sleep(msgsen.exec.delay);
                    
                    gettimeofday(&msgsen.exec.ini, NULL);
                    printf("Job: %d - Name: %s sendo repassado ao escalonador\n", msgsen.exec.job, msgsen.exec.name);
                    msgsnd(idfila, &msgsen, sizeof(struct mensagem), 0);
                    exit(0);
                }
            }
        }
    }
}
