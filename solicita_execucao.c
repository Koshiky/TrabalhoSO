#include  "utilizados.h"

int main(int argc, char* argv[]) {
    int delay, n, p, shmid, idfila, *idjob;
    struct mensagem rcv, exe;

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
          }

          strcpy(exe.exec.name, argv[3]);
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
          }

          strcpy(exe.exec.name, argv[4]);
    }

    idfila = msgget(KFILA, IPC_CREAT|0600); //Owner pode ler e escrever
    shmid = shmget(KJOB, sizeof(int), IPC_CREAT|0600);
    idjob = (int *) shmat(shmid, NULL, 0);

    //Preenche a struct para execucao
    exe.exec.job = *idjob + 1;
    exe.exec.delay = delay;
    exe.exec.n = n;
    exe.prioridade = p;

    printf("%d\n", idfila);

    msgsnd(idfila, &exe, sizeof(struct mensagem), 1);

    printf("%d %d %d %s\n", exe.exec.delay, exe.exec.n, exe.prioridade, exe.exec.name);

    msgrcv(idfila, &rcv, sizeof(struct mensagem), 1, IPC_NOWAIT);

    printf("%d %d %d %s\n", rcv.exec.delay, rcv.exec.n, rcv.prioridade, rcv.exec.name);


    return 0;
}
