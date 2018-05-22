#include  "utilizados.h"

int main(int argc, char* argv[]) {
    int delay, n, p, idshm, idfila, *idjob, pid;
    struct mensagem msgenv, exe;

    if(argc < 4 || argc > 5){
      printf("Quantidade invalida de argumentos\n");
      return 0;
    }

    else if(argc == 4){

          if(!(delay = atoi(argv[1]))){
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

          strcpy(msgenv.exec.name, argv[3]);
    }

    else if(argc == 5){

          if(!(delay = atoi(argv[1]))){
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

          strcpy(msgenv.exec.name, argv[4]);
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

    msgsnd(idfila, &msgenv, sizeof(struct mensagem), 0);

    printf("%d %d %d %d %s\n", msgenv.exec.job, msgenv.exec.delay, msgenv.exec.n, msgenv.prioridade, msgenv.exec.name);
    msgrcv(idfila, &exe, sizeof(struct mensagem), 0, IPC_NOWAIT);
    printf("%d %d %d %d %s\n",msgenv.exec.job, exe.exec.delay, exe.exec.n, exe.prioridade, exe.exec.name);

    return 0;
}
