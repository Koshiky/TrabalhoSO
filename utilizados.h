#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>

#define DONE  8
#define KFILA 16
#define KJOB  32
#define KQNT  64
#define KPAI  128

struct executavel {
    char name[100];
    double time;
    int job, delay, n;
    struct timeval ini, fim;
};

struct mensagem{
    int prioridade;
    struct executavel exec;
};
