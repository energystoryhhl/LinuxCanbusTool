#ifndef __PYCALL_H__
#define __PYCALL_H__

typedef struct pyCallTask{
    char *fileName;
    char *module;
    char *fun;
}pyCallTask_t;


int PyInit();
int PyReadcsv(char *m);
int PyCallfun(char* module, char* fun, char* arg);
void * PyProcessThread(void* arg);
#endif // __PYCALL_H__
