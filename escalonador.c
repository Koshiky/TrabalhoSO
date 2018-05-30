#include  "utilizados.h"

int *nProc, pidFilho;

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

    printf("/nidfila = %d/n", id);

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

    id = msgget(KFILHO, IPC_CREAT|0600);
    msgctl(id, IPC_RMID, NULL);

    id = msgget(KLISTA, IPC_CREAT|0600);
    msgctl(id, IPC_RMID, NULL);

    id = msgget(KAUX, IPC_CREAT|0600);
    msgctl(id, IPC_RMID, NULL);

    id = shmget(KJOB, sizeof(int), IPC_CREAT|0600);
    shmctl(id, IPC_RMID, NULL);

    id = shmget(KPAI, sizeof(int), IPC_CREAT|0600);
    shmctl(id, IPC_RMID, NULL);

    id = shmget(KQNT, sizeof(int), IPC_CREAT|0600);
    shmctl(id, IPC_RMID, NULL);

    kill(pidFilho, SIGTERM);
    kill(getpid(), SIGTERM);
}

int main(){
    int i, j, shmid, *pidP, *idjob, idfila, idfilho, delay, pid, estado, returnWait;
    char nome[100];
    struct mensagem msgrec, msgsen;

    idfila = msgget(KFILA, IPC_CREAT|0600); //Owner pode ler e escrever

    idfilho = msgget(KFILHO, IPC_CREAT|0600); //Owner pode ler e escrever

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
        i = 1;
        while (1) {
            if (msgrcv(idfilho, &msgrec, sizeof(struct mensagem), i, IPC_NOWAIT) == -1) {
                waitpid(-1, &estado, WNOHANG); //Caso fila vazia, wait para lidar com processo zombie, passando pelas filas de prioridades
                if (i != 3) {
                    i++; // avança pra proxima prioridade
                }else {
                    i = 1; // volta pra prioridade 1
                }
            }
            else {

                i = 1;

                for(j=0;j<msgrec.exec.n;j++) {
                    if((pid = fork()) == 0) {
                        printf("Vou executar\n");
                        if (msgrec.exec.inExecution) { // verificar se já tá rolando
                            kill(msgrec.exec.pid, SIGCONT);
                            exit(0);
                        }else {
                            execl(msgrec.exec.name, "", 0);
                        }
                    }
                    else {

                        if (msgrec.exec.inExecution) { // se o pid já estava rodando, utiliza o pid guardado
                            pid = msgrec.exec.pid;
                        }

                        printf("Antes sleep(quantum)\n");
                        sleep(QUANTUM);
                        returnWait = waitpid(pid, &estado, WNOHANG);

                        if (returnWait == 0) { // processo ainda esta rodando
                            printf("Estorou o Quantum\n");
                            kill(pid, SIGSTOP);
                            msgrec.exec.inExecution = 1;
                            msgrec.exec.pid = pid;

                            if (msgrec.exec.repeatQueue) {
                                msgrec.exec.repeatQueue = 0;
                                if (msgrec.exec.direction == DOWN) {
                                    if(++msgrec.prioridade == 3) {
                                        msgrec.exec.direction = UP;
                                    }
                                    printf("Nova Prioridade: %d\n", msgrec.prioridade);
                                } else {
                                    if(--msgrec.prioridade == 1) {
                                        msgrec.exec.direction = DOWN;
                                    }
                                    printf("Nova Prioridade: %d\n", msgrec.prioridade);
                                }
                            }else {
                                msgrec.exec.repeatQueue = 1;
                            }

                            // mandar pro final da fila
                            msgsnd(idfilho, &msgrec, sizeof(struct mensagem), 0);

                        }else { // processo terminou de rodar
                            printf("Terminou Execução\n");

                            gettimeofday(&msgrec.exec.fim, NULL);
                            msgrec.exec.time = msgrec.exec.fim.tv_sec - msgrec.exec.ini.tv_sec;
                            msgrec.exec.time += (msgrec.exec.fim.tv_usec - msgrec.exec.ini.tv_usec) / 1000000.0;
                            *nProc = *nProc - 1;
                            msgrec.prioridade = DONE;
                            msgsnd(idfila, &msgrec, sizeof(struct mensagem), 0);

                            printf("Job: %d Delay: %d N: %d Priority: %d Name: %s\n", msgrec.exec.job, msgrec.exec.delay, msgrec.exec.n, msgrec.prioridade, msgrec.exec.name);
                        }
                    }
                }
            }

        }
    }

    pidFilho = pid;
    //Espera pelo shutdown
    signal(SIGUSR1, shutdown);

    /*----------------------------------------------------*/
    //Rotina do pai
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

                    gettimeofday(&msgsen.exec.ini, NULL);
                    sleep(msgsen.exec.delay/10);

                    printf("Job: %d - Name: %s sendo repassado ao escalonador\n", msgsen.exec.job, msgsen.exec.name);

                    msgsnd(idfilho, &msgsen, sizeof(struct mensagem), 0);
                    exit(0);
                }
            }
        }
    }
}
