#include "utilizados.h"

int main(int argc, char const *argv[]) {

  int idfila, idlista, idaux, job;
  struct mensagem msg;

  if(argc != 2){
      printf("Quantidade invalida de argumentos\n");
      return 0;
  }

  else if(!(job = atoi(argv[1]))){
      printf("Job invalido\n");
      return 0;
  }

  idfila = msgget(KFILA, IPC_CREAT|0600);
  idlista = msgget(KLISTA, IPC_CREAT|0600);
  idaux = msgget(KAUX, IPC_CREAT|0600);

  printf("%d\n", job);

  while(msgrcv(idlista, &msg, sizeof(struct mensagem), 0, IPC_NOWAIT) != -1){}

  while(msgrcv(idaux, &msg, sizeof(struct mensagem), 0, IPC_NOWAIT) != -1){}

  while(msgrcv(idfila, &msg, sizeof(struct mensagem), 0, IPC_NOWAIT) != -1){
      if(msg.exec.job != job) {
          msgsnd(idaux, &msg, sizeof(struct mensagem), 0);
      }
      printf("Job %d Exec %d/n", job, msg.exec.job);
  }

  while(msgrcv(idaux, &msg, sizeof(struct mensagem), 0, IPC_NOWAIT) != -1){
          //msgsnd(idfila, &msg, sizeof(struct mensagem), 0);
          msgsnd(idlista, &msg, sizeof(struct mensagem), 0);
  }

  return 0;
}
