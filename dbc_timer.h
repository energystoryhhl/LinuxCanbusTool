#ifndef __DBC_TIMER_H__
#define __DBC_TIMER_H__

#define _10msTask  1
#define _20msTask  2
#define _50msTask  3
#define _100msTask 4
#include "canbus.h"

typedef struct{

    pthread_cond_t _10msThreadReady;
    pthread_cond_t _20msThreadReady;
    pthread_cond_t _50msThreadReady;
    pthread_cond_t _100msThreadReady;
    int threadStop;
    pthread_mutex_t timerThreadLock;
    pthread_t *timerRoutineThread;

} TimerThread_t;

typedef struct taskArg{
    canBus_t * can;
    unsigned int boId;
}taskArg_t;

typedef struct TaskAdd{
    void(*task)(taskArg_t *);
    int canID;
    taskArg_t arg;
}TaskAdd_t;

TimerThread_t* TimerRoutineInit(void);

TimerThread_t* timerThread;


/*
Task->task is the thread to be run
Task->canID is the parameter of the task
*/
pthread_t AddTimerTask(TaskAdd_t * Task, int Type);

void TimerCtl(int on_off);

//void _100MsTask(void (*fun)(void));

#endif // __DBC_TIMER_H__
