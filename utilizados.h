#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/time.h>
#include <unistd.h>

#define KFILA 16
#define KJOB  32
#define KQNT  64
#define KPAI  128
#define DONE  8

struct executavel {
    struct timeval ini, fim;
    int job, delay, n;
    double time;
    char name[100];
};

struct mensagem{
    struct executavel exec;
    int prioridade;
};
