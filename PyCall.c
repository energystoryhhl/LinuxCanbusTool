#include "main.h"
#include "PyCall.h"
#include <sys/stat.h>
#include <pthread.h>

#define TRIGESIZE 1  //mb
#define DETECTETIME 30 //s

int FileSize(char *filename)
{
    struct stat statbuf;
    int ret;
    ret = stat(filename, &statbuf);
    if(ret != 0)
        return -1;
    return statbuf.st_size;
}

void PythonCall(pyCallTask_t* t)
{
    size_t size = 0;
    while(1)
    {
        size = FileSize(t->fileName);
        printf("the data size is: %ld bytes\n", size);
        if(size > TRIGESIZE*1024*1024);
        {
            printf("file size triged!\n");
            FILE* file = fopen(t->fileName,"w+");//clear the file
            fclose(file);
            size = 0;
        }
        sleep(DETECTETIME);
    }
}

void * tasktest(void *arg)
{
    FILE * testfile = fopen("testFile.txt","w+");
    printf("write thread start!\n");
    while(1)
    {
        fprintf(testfile,"this is just test!!!!!!!!!!!!!\n");



    }
}


int StartPyCall()
{
    pyCallTask_t task;
    pthread_t testTask;
    pthread_create(&testTask,NULL,tasktest,NULL);

    //task.fileName = "testFile.txt";
    //PythonCall(&task);

    while(1);
    return 0;
}

