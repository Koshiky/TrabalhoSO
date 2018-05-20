#include  "utilizados.h"

int main(int argc, char* argv[]) {
    int delay, n, p=1, shmid, idfila, *idjob;
    struct exec exe;

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

          strcpy(exe.name, argv[3]);
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

          strcpy(exe.name, argv[4]);
    }

    idfila = msgget(KFILA, IPC_CREAT|0600); //Owner pode ler e escrever
    shmid = shmget(KJOB, sizeof(int), IPC_CREAT|0600);
    idjob = (int *) shmat(shmid, NULL, 0);

    //Preenche a struct para execucao
    exe.job = *idjob + 1;
    exe.delay = delay;
    exe.n = n;
    exe.prioridade = p;

    msgsnd(idfila, &exe, sizeof(struct exec), 0);

    printf("%d %d %d %s\n", exe.delay, exe.n, exe.prioridade, exe.name);

    return 0;
}
