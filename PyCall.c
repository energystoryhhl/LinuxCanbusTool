#include "main.h"
#include "PyCall.h"
#include "stdio.h"
#include <sys/stat.h>
#include <pthread.h>
#include <python3.5m/ImDib.h>
#include <python3.5m/ImPlatform.h>
#include <python3.5m/Imaging.h>

#define CSV_MAX_LINE_SIZE 4096

#define PYSHOW 1


int FileSize(char *filename)
{
    struct stat statbuf;
    int ret;
    ret = stat(filename, &statbuf);
    if(ret != 0)
        return -1;
    return statbuf.st_size;
}

/*init python*/
int PyInit()
{
    Py_Initialize();
    PyRun_SimpleString("import sys");
    PyRun_SimpleString("sys.path.append('./')");
    //pyThreadRun = 1; //enable pyThread
    return 0;
}

/*let pythin read .csv fiel*/
int PyReadcsv(char *m)
{
    PyObject *pModule = NULL, *pArg = NULL, *result = NULL, *pFunc =NULL;
    pModule = PyImport_ImportModule(m);//import *.py

    pFunc = PyObject_GetAttrString(pModule, "testprint");
    pArg = Py_BuildValue("(s)","just for show!");
    PyObject_CallObject(pFunc,pArg);
//    pFunc = PyObject_GetAttrString(pModule, "xprint");//get .py function
//    pArg = Py_BuildValue("(filename)","testfile.csv");//build arg
//    result = PyObject_CallFunction(pFunc,NULL);//execute py function
    int ret;
    PyArg_Parse(result, "i", &ret);
    printf(">>>%d\n",ret);
    return 0;
}

/*
the function to call py function
module: *.py name
fun: the function of .py
arg:the arguement of fun Attention:must be str/char*
*/
int PyCallfun(char* module, char* fun, char* arg)
{
    PyObject *pModule = NULL, *pArg = NULL, *result = NULL, *pFunc =NULL;
    int ret;
    pModule = PyImport_ImportModule(module);//import *.py
    pFunc = PyObject_GetAttrString(pModule, fun);
    pArg = Py_BuildValue("(s)",arg);
    result = PyObject_CallObject(pFunc,pArg);
    PyArg_Parse(result, "i", &ret);
    return ret;
}

double readCsv(char* csvName, char* indexName)
{
    int counter = 0;
    char line[CSV_MAX_LINE_SIZE];
    FILE * f;
    if(NULL == (f = fopen(csvName,"r+")))
    {
        printf("csv open error!\n");
        return -1;
    }
    fgets(line, CSV_MAX_LINE_SIZE, f);
    char *p = strtok(line, ",");
    if(p)
    {
        //printf(">>>%s\n",p);
        counter++;
        if(!strcmp(p,indexName))
            goto process;
    }
    else
    {
        printf("open csv err\n");
        return -1;
    }
    while(1)
    {
        p = strtok(NULL,",");
        if(p)
        {
            counter++;
            //printf(">>>%d",counter);
            //printf("%s\n",p);
            if(!strcmp(p,indexName))
                break;
        }
        else
        {
            break;
        }
    }
process:
    memset(line,0,CSV_MAX_LINE_SIZE);
    fgets(line, CSV_MAX_LINE_SIZE, f);
    p = strtok(line, ",");
    if(p)
        counter--;
    int i ;
    for(i = 0;i<counter;i++)
    {
        p = strtok(NULL,",");
        //printf(">>%s\n",p);
    }
    double ret = 0;
    ret = atof(p);
    fclose(f);
    return ret;
}



int readTrain()
{
    printf(">>>P:%lf\n",readCsv("train.csv","P"));
    printf(">>>I:%lf\n",readCsv("train.csv","I"));
    printf(">>>D:%lf\n",readCsv("train.csv","D"));

    return 0;
}


void * PyProcessThread(void* arg)
{

    pyCallTask_t *pytask = (pyCallTask_t*)arg;
    int ret = PyCallfun(pytask->module, pytask->fun, pytask->fileName);

    #ifdef PYSHOW
    printf("ret is %d\n",ret);
    printf("PY done!\n");

    #endif // PYSHOW
    readTrain();
    return NULL;
}











