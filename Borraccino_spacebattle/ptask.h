/*					TASK.H					*/

#include <time.h>
#include <pthread.h>
#include <stdio.h>

#define MAXT   10   // max number of tasks

struct task_par {
    int     arg;
    long    wcet;
    int     period;
    int     deadline;
    int     priority;
    int     dmiss;
    struct  timespec at;
    struct  timespec dl;
};


struct sched_param	mypar;
struct task_par	tp[MAXT];
pthread_attr_t		att[MAXT];
pthread_t		tid[MAXT];

void time_copy(struct timespec *td, struct timespec ts);

void time_add_ms(struct timespec *t, int ms);

int time_cmp(struct timespec t1, struct timespec t2);


int get_task_index(void* arg);
void set_activation(int i);
int deadline_miss(int i);
void wait_for_activation(int i);

int task_create(
	void*(*task)(void *),
	int i,
	int period,
	int drel,
	int prio
	);

/***************************** END OF FILE **********************************/
/****************************************************************************/
/* 	SPACEBATTLE GAME, RTS PROJECT, DEVELOPED BY MARCO BORRACCINO        */
/****************************************************************************/
