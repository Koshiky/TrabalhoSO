#include "utilizados.h"

int main(int argc, char const *argv[]) {

  int idlista, idaux, job;
  struct mensagem msg;

  if(argc != 2){
      printf("Quantidade invalida de argumentos\n");
      return 0;
  }

  else if(!(job = atoi(argv[1]))){
      printf("Job invalido\n");
      return 0;
  }

  idlista = msgget(KLISTA, IPC_CREAT|0600);
  idaux = msgget(KAUX, IPC_CREAT|0600);

  printf("%d\n", job);

  while(msgrcv(idlista, &msg, sizeof(struct mensagem), 0, IPC_NOWAIT) != -1){
		if(msg.exec.job != job) {
        	msgsnd(idaux, &msg, sizeof(struct mensagem), 0);
      	}else {
			printf("Job %d Removido/n", job);
			kill(SIGKILL, msg.exec.pid);
	  	}
  }

  while(msgrcv(idaux, &msg, sizeof(struct mensagem), 0, IPC_NOWAIT) != -1){
          //msgsnd(idfila, &msg, sizeof(struct mensagem), 0);
          msgsnd(idlista, &msg, sizeof(struct mensagem), 0);
  }

  return 0;
}
