#include  "utilizados.h"

int main(int argc, char* argv[]) {
    int delay=0, n, p, idshm, idfila, *idjob;
    struct mensagem exe, msgenv;
    char* token;

    if(argc < 4 || argc > 5){
        printf("Quantidade invalida de argumentos\n");
        return 0;
    }

    else if(argc == 4){

        token = strtok(argv[1],":");
        if (token != NULL) {
            delay += atoi(token)*3600;
            token = strtok(NULL, "");
            if (token != NULL) {
                delay += atoi(token)*60;
            }
        }
        if(!delay){
            printf("Delay invalido\n");
            return 0;
        }

        if(!(n = atoi(argv[2]))){
            printf("Numero de vezes a serem executadas invalido\n");
            return 0;
        }

        p = 1;

        if (strlen(argv[3]) <= 0) {
            printf("Nome do executavel invalido\n");
            return 0;
        }
        else strcpy(msgenv.exec.name, argv[3]);
    }

    else if(argc == 5){

        token = strtok(argv[1],":");
        if (token != NULL) {
            delay += atoi(token)*3600;
            token = strtok(NULL, "");
            if (token != NULL) {
                delay += atoi(token)*60;
            }
        }
        if(!delay){
            printf("Delay invalido\n");
            return 0;
        }

        if(!(n = atoi(argv[2]))){
            printf("Numero de vezes a serem executadas invalido\n");
            return 0;
        }

        if(!(p = atoi(argv[3]))){
            printf("Numero de prioridade invalido\n");
            return 0;
        }
        else if (p < 1 || p > 3){
            printf("Numero de prioridade invalido\n");
            return 0;
        }

        if (strlen(argv[4]) <= 0) {
            printf("Nome do executavel invalido\n");
            return 0;
        }
        else strcpy(msgenv.exec.name, argv[4]);
    }

    idfila = msgget(KFILA, IPC_CREAT|0600); //Owner pode ler e escrever

    idshm = shmget(KJOB, sizeof(int), IPC_CREAT|0600);
    idjob = (int *) shmat(idshm, NULL, 0);

    //Preenche a struct para execucao
    *idjob = *idjob + 1;
    msgenv.exec.job = *idjob;
    msgenv.exec.delay = delay;
    msgenv.exec.n = n;
    msgenv.prioridade = p;
	msgenv.exec.inExecution = 0;
	msgenv.exec.pid = 0;
	msgenv.exec.repeatQueue = 0;
	msgenv.exec.direction = 0;

    msgsnd(idfila, &msgenv, sizeof(struct mensagem), 0);

    printf("Job: %d Delay: %d N: %d Priority: %d Name: %s\n", msgenv.exec.job, msgenv.exec.delay, msgenv.exec.n, msgenv.prioridade, msgenv.exec.name);

    return 0;
}
