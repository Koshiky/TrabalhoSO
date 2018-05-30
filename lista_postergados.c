#include "utilizados.h"
#include <time.h>
#define SEG_POR_DIA   86400
#define SEG_POR_HORA  3600
#define SEG_POR_MIN   60

int main() {

  int idlista, job;
  int hora, minutos;
  long horario, aux=0;
  struct mensagem msg;
  struct timeval tv;
  struct timezone tz;

  idlista = msgget(KLISTA, IPC_CREAT|0600); //Owner pode ler e escrever

  gettimeofday(NULL, &tz);

  //Print dos Jobs
  while(msgrcv(idlista, &msg, sizeof(struct mensagem), 0, IPC_NOWAIT) != -1){
      if(msg.exec.job != job) {
          printf("\n");
          job = msg.exec.job;
      }

      horario = msg.exec.ini.tv_sec % SEG_POR_DIA;
      horario += tz.tz_dsttime * SEG_POR_HORA;
      horario -= tz.tz_minuteswest * SEG_POR_MIN;
      horario = (horario + SEG_POR_DIA) % SEG_POR_DIA;
      aux = horario + msg.exec.delay;

      // Separa em h:m:s
      hora = aux / SEG_POR_HORA;
      minutos = (aux % SEG_POR_HORA) / SEG_POR_MIN;

      printf("Job: %d Executavel: %s Horario: %d:%02d Copias: %d Prioridade: %d\n", msg.exec.job, msg.exec.name, hora, minutos, msg.exec.n, msg.prioridade);
  }

  return 0;
}
