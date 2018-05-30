# Gerente de Escalonamento Postergado de Processos CPU/bound
### Renato Estevam Nogueira 13/0036579
### Roberto Nishino Ono 10/0122272
----
##### Processos
* solicita\_execucao \<hora:min> \<cópias> \<pri> \<arq_executável> 
	* \<hora:min> : delay de execução em relação à hora corrente (obrigatório)
	* \<cópias> : número de cópias que serão executadas (obrigatório)
	* \<pri> : prioridade inicial do processo
	* \<arq_executável> : nome do arquivo que será executado de maneira postergada (obrigatório)

* escalonador
* remove\_postergado \<id\_job>
	* \<id\_job> : ID do job que será removido
* lista_postergados
* shutdown

----
#### Mecanismos de Comunicação Interprocessos
* Filas de Mensagens
* Memória Compartilhada

----
#### Estruturas de Dados
```c
struct executavel {
    char name[100];
    double time;
    int job, delay, n, pid, inExecution, repeatQueue, direction;
    struct timeval ini, fim;
}
```
> name = nome do executável

> time = tempo de execução

> job = ID de job

> delay = tempo de delay

> N = número de cópias do processo

> pid = pid do processo

> inExecution = identifica se o processo vai rodar pela primeira vez ou se está parado devido a um quantum

> repeatQueue = contador para verificar se o processo já passou 2 vezes pela mesma prioridade

> direction = identifica se o processo está subindo ou descendo de prioridade

> ini = struct com valor de tempo do inicio do processo

> fim = struct com valor de tempo do final do processo 

```c
struct mensagem{
    int prioridade;
    struct executavel exec;
};
```

> prioridade = qual a fila de prioridade

> exec = struct com os valores da mensagem