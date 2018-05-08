#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <pthread.h>
#include <sys/types.h>

#include "BO.h"
#include "BO_Index.h"

#include "menu.h"
#include "canbus.h"
#include "main.h"
#include "pharse.h"
#include "package.h"
#include "Tx2TryDBC_define.h"
#include "dbc_timer.h"
#include "MySql.h"


FILE * dbc_cfg_fd = NULL;       //dbc file fd
char *dbc_cfg_rdbuf = NULL;     //dbc read buffer
int can_fd;                     //can 设备句柄
int can_fd2;                    //can设备句柄
struct can_frame frame;         //recv frame    接收buffer
struct can_frame frame_send;    //frame to be send发送buffer

pthread_t printThread;          //打印线程的线程句柄
/*统计时间的变量*/
long read_time ;
long parse_time ;
long bypass_time ;
long pack_time ;
long write_time ;
int count;

/*打印线程*/
void PrintTask(void)
{
    while(1)
    {
                    system("clear");
                    int i =0;
                    while(BO_List[i] != NULL)
                    {
                        printf("BO_ %d %s\n",BO_List[i]->message_id,BO_List[i]->message_name);
                        int t = 0;
                        while(BO_List[i]->SG_List[t] != NULL) /*SG_LOOP*/
                        {
                            printf("%s",BO_List[i]->SG_List[t]->signal_name);
                            printf(">>>%-15lf",BO_List[i]->SG_List[t]->value);
                            printf("send>%-15lf\t",BO_List[i]->SG_List[t]->value_send);
                            if((t+1)%2 == 0)
                            {
                                printf("\n");
                            }
                            t ++;
                        }
                        printf("------------------------\n");
                        i++;
                    }
//                    printf(">>>>read   time:%ld us<<<<\n",read_time);
//                    printf(">>>>parse  time:%ld us<<<<\n",parse_time);
//                    printf(">>>>bypass time:%ld us<<<<\n",bypass_time);
//                    printf(">>>>pack   time:%ld us<<<<\n",pack_time);
//                    printf(">>>>write  time:%ld us<<<<\n",write_time);
//                    printf("meg received : %d\n",count);
//                    printf("MASTER_REQ_RollingCounter:%lf\n",MASTER_REQ_RollingCounter);
                    usleep(10*1000);
    }
}

int main(int argc,char *argv[])
{
    //struct timeval start, end;
    int dbc_falg = 0;           //dbc文件载入标志位
    dbc_cfg_rdbuf = malloc(100);//dbc文件读取buffer
    int BO_counter = 0;         //dbc中的BO计数器
    memset(BO_List,0,4*50);     //初始化

    printf("******************* dbc_load_program V0.2 *********************\n");
    printf("******************* By intern huang honglei *******************\n");
    printf("*******************       2018/4/23         *******************\n");

    if(argc != 1)
    {
        if(NULL == (dbc_cfg_fd = fopen(argv[1],"r")) )
        {
            printf("***dbc config file open failed!\n");
            printf("Now no dbc file is loaded\n");
        }
        else
        {
            Dbc_load(dbc_cfg_fd,dbc_cfg_rdbuf,BO_List);//读取dbc文件
            printf(">>>dbc config file loaded\n");
            dbc_falg = 1;
        }
    }
    else if(argc == 1)
    {
        printf("Now no dbc file is loaded\n");
        printf("No interface is loaded!\n");
        print_usage();
    }

    printf("Press Any Key \n");
    getchar();
    system("clear");

    /*print state*/
    if(1 == dbc_falg)
    {
        printf("Canbus Mode:%s\t\n",argv[1]);
    }


    /*choose打印菜单*/
    printf("1:Canbus MySql\n2:Fork\n  -->can0;can1-->\n  <--can0;can1<--\n3:-->can0;can1-->\n  <--can0;can1<--\n");
    //int nbytes;
    int choice;
    scanf("%d",&choice);

    canbuscmd_t cmd;
    cmd.interface = "can0";
    cmd.record = 1;
    cmd.show = 1;
    cmd.loopback = 0;
    switch(choice)
    {
        case 1:
            CanbusRecvRecord(&cmd);
            RecordDataCsv();
            break;

        case 2:
                PthreadCanbusTest();//can0和can1回环功能测试
            break;

        case 3:
                CanBusTestMode();//10ms 20ms 50ms 100ms程序测试
            break;

    }
    while(1);
    /*free malloc*/
    Free_BO_(BO_counter);
    return 0;
}

