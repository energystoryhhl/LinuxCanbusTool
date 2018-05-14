/**********************************************************
*Author : huanghonglei
*Date   : 2018/4/27
*function: through MySql record the canbus Data
***********************************************************/
#include "MySql.h"
#include "PyCall.h"
#include "GetTime.h"
#include "canbus.h"
#include "pharse.h"
#include "sys/time.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>


#define STEPTIME 100 //ms

#define CSV_NAME_SIZE 50
#define MAX_FILE_NUMBER 3
#define DATATRIGSIZE 200 //

long long sqlCounter = 1;
const char *g_host_name = "localhost";
const char *g_user_name = "root";
const char *g_password = "nvidia";
const char *g_db_name = "test";
const unsigned int g_db_port = 3306;

struct timeval t_start;
struct timeval t_end;

void print_mysql_error(const char *msg)
{
    if(msg)
        printf("%s:%s\n", msg, mysql_error(g_conn));
    else
        puts(mysql_error(g_conn));
}

/*execute the MySql language
const char *sql : the MySql language to be executed
*/
int executesql(const char *sql)
{
    if(mysql_real_query(g_conn, sql,strlen(sql)))
        return -1;
    return 0;
}

int init_mysql()
{
    g_conn = mysql_init(NULL);
    if(!mysql_real_connect(g_conn, g_host_name, g_user_name, g_password, g_db_name, g_db_port, NULL, 0))
        return -1;

    if(executesql("set names utf8"))
        return -1;

    return 0;
}

int CreateTable(char* sqlbuf, char * tableName, BO_Unit_t **boList)
{
    int i = 0;
    int j = 0;
    /*Create an new table named by date and time */
    sprintf(sqlbuf, "create table t_%s (id int(32) primary key auto_increment not null);", tableName);
    printf(">>>%s\n",sqlbuf);
    if(executesql(sqlbuf))
        print_mysql_error(NULL);
    memset(sqlbuf, 0, MAX_BUF_SIZE);


    /*iterate the boList to establish the table*/
    while(boList[i])
    {
        printf(">>>%s\n",boList[i]->message_name);
        while(boList[i]->SG_List[j])
        {
            printf("  %s\n",boList[i]->SG_List[j]->signal_name);
            sprintf(sqlbuf, "alter table t_%s add %s double(6,2)", tableName, boList[i]->SG_List[j]->signal_name);
            if(executesql(sqlbuf))
                print_mysql_error(NULL);
            memset(sqlbuf, 0, MAX_BUF_SIZE);
            j++;
        }
        j = 0;
        i++;
    }
    return 1;
}

void CanbusRecord(mySqlTask_t *taskArg)
{
    int i = 0;
    int j = 0;
    sprintf(taskArg->sqlbuf, "insert into t_%s (id) values(%lld);",taskArg->tableName,sqlCounter);
    if(executesql(taskArg->sqlbuf))
        print_mysql_error(NULL);
    memset(taskArg->sqlbuf, 0, MAX_BUF_SIZE);
    while(taskArg->boList[i])
    {
        //printf(">>>%s\n",taskArg->boList[i]->message_name);
        while(taskArg->boList[i]->SG_List[j])
        {
            //printf("  %s\n",taskArg->boList[i]->SG_List[j]->signal_name);
            sprintf(taskArg->sqlbuf, "update t_%s set %s = %.2lf where id = %lld", taskArg->tableName, taskArg->boList[i]->SG_List[j]->signal_name, taskArg->boList[i]->SG_List[j]->value,sqlCounter);
            if(executesql(taskArg->sqlbuf))
            {
                print_mysql_error(NULL);
                //printf("ERR>>> %s\n<<",taskArg->sqlbuf);
            }
            memset(taskArg->sqlbuf, 0, MAX_BUF_SIZE);
            j++;
        }
        j = 0;
        i++;
    }
    sqlCounter++;
}


extern void PrintTask(void);
/*mysql record test program*/
void CanbusMySqlTest()
{
    pthread_t showThread;
    pthread_create(&showThread, NULL, (void *)PrintTask, NULL);   //创建打印线程
    char date[30]; //date is the TableName!!!
    char sql[MAX_BUF_SIZE];
    mySqlTask_t task;
    task.boList = BO_List;
    task.sqlbuf = sql;
    task.tableName = date;

    /*Get current Time*/
    GetTime(&timeSaved,timeDate,date);
    printf("Date>>>%s",date);

    /*init MySql*/
    if(init_mysql());
        print_mysql_error(NULL);

    /*Create a blank table*/
    CreateTable(sql, date, BO_List);
    int nbytes = 0;
    canBus_t *CAN0 = CanBusInit("can0");//初始化can设备
    int counter = 0;
    while(1)
    {
        gettimeofday(&t_start,NULL);
        nbytes = read(CAN0->canFd, &CAN0->recvBuffer, sizeof(struct can_frame));
        if(nbytes > 0)
        {
            parse3(&CAN0->recvBuffer,BO_List);
            CanbusRecord(&task);
            counter ++;
            //printf("sum = %d\n",counter);
        }
        gettimeofday(&t_end,NULL);
        //printf("time>> %ld,%ld\n",t_end.tv_sec - t_start.tv_sec,t_end.tv_usec - t_start.tv_usec );
        //usleep(10*1000);
    }
}

void CreateCsv(FILE ** f,char *fname)
{
    *f = fopen(fname,"w+");
    int i = 0, j = 0;
    //Create csv list tittle
    while(BO_List[i])
    {
        //printf(">>>%s\n",taskArg->boList[i]->message_name);
        while(BO_List[i]->SG_List[j])
        {
            fwrite(BO_List[i]->SG_List[j]->signal_name,1,strlen(BO_List[i]->SG_List[j]->signal_name),*f);
            fwrite(",",1,1,*f);
            //printf("write once\n");
            j++;
        }
        j = 0;
        i++;
    }
    fwrite("\n",1,1,*f);
}
//void RecordData(FILE ** f)
void RecordData(FILE ** f)
{
    //printf("record thread starting!\n");
    int i = 0, j = 0;
        //Create csv list tittle
        while(BO_List[i])
        {
            //printf(">>>%s\n",taskArg->boList[i]->message_name);
            while(BO_List[i]->SG_List[j])
            {
                fprintf(*f,"%f",BO_List[i]->SG_List[j]->value);
                fwrite(",",1,1,*f);
                //printf("write once\n");
                j++;
            }
            j = 0;
            i++;
        }
        fwrite("\n",1,1,*f);
        usleep(STEPTIME*1000);
}

void * CsvThread(void *arg)
{
    csvTask_t* argIn = (csvTask_t*)arg;

    FILE ** f = argIn->fptr;
    int i = 0, j = 0;
    while(1)
    {
       while(BO_List[i])
        {
            //printf(">>>%s\n",taskArg->boList[i]->message_name);
            while(BO_List[i]->SG_List[j])
            {
                int a;
                a = fprintf(*f,"%f",BO_List[i]->SG_List[j]->value);
                printf("write :%d\n",a);
                //fwrite(",",1,1,recordFile);
                fprintf(*f,",");
                j++;
            }
            j = 0;
            i++;
        }
        fwrite("\n",1,1,*f);
        usleep(STEPTIME*1000);
    }
}

int AddCsvFile(csvFileList_t* list,char* fileName)
{
    if(list->counter < MAX_FILE_NUMBER)
    {
        strcpy(list->fileNameList[list->counter],fileName);
        list->counter++;
    }
    if(list->counter == MAX_FILE_NUMBER)
    {
        if(-1 == remove(list->fileNameList[0]))
            printf("remove file error!");
        int i;
        for(i = 0; i< list->counter; i++)
        {
            strcpy(list->fileNameList[i],list->fileNameList[i+1]);
        }
        strcpy(list->fileNameList[list->counter - 1],fileName);
        list->counter--;
    }
    return 0;
}



void RecordDataCsv()
{
    long counter = 0;
    unsigned short csvFileCounter = 0;
    char csvName[CSV_NAME_SIZE]; //date is the TableName!!!
    csvFileList_t csvList;
    memset(&csvList, 0, sizeof(csvList));
    pthread_t pytask;
    pyCallTask_t pyTask;
    GetTime(&timeSaved, timeDate, csvName);
    strcat(csvName,".csv");
    //sprintf(csvName, "%s.csv", csvName);
    if(0 == access(csvName,0))
    {
        strtok(csvName,".");
        strcat(csvName,"_new.csv");
        CreateCsv(&recordFile,csvName);
        AddCsvFile(&csvList,csvName);
        csvFileCounter ++;
    }
    else
    {
        CreateCsv(&recordFile,csvName);
        AddCsvFile(&csvList,csvName);
        csvFileCounter++;
    }

    pyTask.fileName = (char *)malloc(50);
//    pthread_t recordThread;
//    pthread_create(&recordThread,NULL,RecordData,&recordFile);
//    csvTask_t csvarg;
//    csvarg.fptr = &recordFile;
//
//    pthread_t csvThread;
//    pthread_create(&csvThread,NULL,CsvThread,&csvarg);
    while(1)
    {
        RecordData(&recordFile);
        counter ++;

        system("clear");
        printf("size is %ld\n",counter);
        fflush(stdout);

        if(counter >= DATATRIGSIZE)
        {
            //printf("size is triged!\n");
            counter = 0;
            fclose(recordFile); //close file!
            printf("call py funciotn!\n");
            /*cal py*/
            //pyTask.fileName = csvName;
            strcpy(pyTask.fileName,csvName);
            pyTask.fun = "readcsv";
            pyTask.module = "test01";
            pthread_create(&pytask,NULL,PyProcessThread,&pyTask);

            //memset(&pyTask,0,sizeof(pyTask));
            //PyCallfun("test01", "readcsv", csvName);
            printf("done!\n");

            /*done*/
            memset(csvName,0,CSV_NAME_SIZE);
            GetTime(&timeSaved, timeDate, csvName); //create new file
            strcat(csvName,".csv");
            if(0 == access(csvName,0))
            {
                strtok(csvName,".");
                strcat(csvName,"_new.csv");
                CreateCsv(&recordFile,csvName);
                AddCsvFile(&csvList,csvName);
                csvFileCounter++;
            }
            else
            {
                CreateCsv(&recordFile,csvName);
                AddCsvFile(&csvList,csvName);
                csvFileCounter++;
            }
            sleep(10);
        }
    }

}








