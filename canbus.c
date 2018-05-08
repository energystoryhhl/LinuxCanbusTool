/******************************************************************
canbus.c
创建者：黄洪磊
时间：2018/4/23
功能：实现can设备收发
*******************************************************************/
#include "main.h"
#include "canbus.h"
#include "BO.h"
#include "pharse.h"
#include <pthread.h>
#include "dbc_timer.h"
#include "package.h"



unsigned long min_num=0;            //随机发送最小值
unsigned long max_num=0xffffffff;   //随机发送最大值
unsigned long random_num;           //随机数
unsigned long random_num_h ;        //随机数高位
unsigned long random_num_l ;        //随机数低位

extern struct can_frame frame ;     //发送buffer

/*
can设备初始化函数
char *interface：设备接口名 如"can0"
*/
int SocketCAN_init(char *interface)
{
    //int enable_canfd = 1;

    //struct can_frame frame ;
    int can_fd;
    //frame_ptr = (struct can_frame *)malloc(sizeof(struct can_frame));
    struct sockaddr_can addr;
    struct ifreq ifr;
    if((can_fd = socket(PF_CAN,SOCK_RAW,CAN_RAW))<0)
    {
        printf("socket err!\n");
    }

    strcpy(ifr.ifr_name,interface);
    if(-1 == (ioctl(can_fd,SIOCGIFINDEX,&ifr)))
    {
        printf("ioctl err!\n");
    }

	strncpy(ifr.ifr_name, interface, IFNAMSIZ - 1); //pass the interface to the ifr
	ifr.ifr_name[IFNAMSIZ - 1] = '\0';
	ifr.ifr_ifindex = if_nametoindex(ifr.ifr_name); //check if the interface really exists
	if (!ifr.ifr_ifindex) {
		perror("if_nametoindex");
		return 1;
	}


    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;
    /*enable fd can*/
    //    if (setsockopt(can_fd, SOL_CAN_RAW, CAN_RAW_FD_FRAMES,
    //			       &enable_canfd, sizeof(enable_canfd))){
    //        printf("error when enabling CAN FD support\n");
    //        return 1;
    //    }
    if((bind(can_fd,(struct sockaddr*)&addr,sizeof(addr))) < 0)
    {
        printf("bind err!\n");
    }
    return can_fd;
}

/*
can设备随机发送函数
struct can_frame * frameptr：发送buffer
int can_fd：can设备句柄
*/
int CAN_Send_Random(struct can_frame * frameptr,int can_fd)
{
    int i;
    int sended_msg_num = 0;
    int error_msg_num = 0;
    frameptr->can_id = 0x123;
    frameptr->can_dlc = 8;
    //ptr convert
    unsigned long *tmp = (unsigned long *)(frameptr->data);
    //create random number
    random_num_h = (rand()%(max_num-min_num+1)+min_num);
    random_num_l = (rand()%(max_num-min_num+1)+min_num);

    random_num = ((random_num_h << 32) | random_num_l);
    //by pass the randomnum
    *tmp = random_num;
    //transmit the msg
    int nbytes = write(can_fd,frameptr,sizeof(*frameptr));

    //for show
    //reverse the msg!
    if(nbytes == sizeof(*frameptr))
    {
        for(i = 0;i<4;i++)
        {
                unsigned char show_num = random_num_l >> 8*i;
                printf("%X " , show_num);
        }
        printf("\t");
        for(i = 0;i<4;i++)
        {
                unsigned char show_num = random_num_h >> 8*i;
                printf("%X " , show_num);
        }
        printf("\n");
        sended_msg_num++;

    }
    if(nbytes != sizeof(*frameptr))
    {
        #ifdef SNDDEBUG
        printf("send err!");
        printf("nbytes is :%d\n",nbytes);
        #endif
        error_msg_num ++;
    }
    return 0;
}

/*
canbus初始化函数
char *interface：can设备接口名
canBus_t * ：返回的canbus结构体
*/
canBus_t * CanBusInit(char *interface)
{
    canBus_t *canbus_ptr = (canBus_t *)malloc(sizeof(canBus_t));
    memset(canbus_ptr, 0, sizeof(canBus_t));
    pthread_mutex_init(&canbus_ptr->rdwrLock, NULL);
    struct sockaddr_can addr;
    struct ifreq ifr;
    if((canbus_ptr->canFd = socket(PF_CAN,SOCK_RAW,CAN_RAW))<0)
    {
        printf("socket err!\n");
    }
    strcpy(ifr.ifr_name,interface);
    if(-1 == (ioctl(canbus_ptr->canFd,SIOCGIFINDEX,&ifr)))
    {
        printf("ioctl err!\n");
    }

	strncpy(ifr.ifr_name, interface, IFNAMSIZ - 1); //pass the interface to the ifr
	ifr.ifr_name[IFNAMSIZ - 1] = '\0';
	ifr.ifr_ifindex = if_nametoindex(ifr.ifr_name); //check if the interface really exists
	if (!ifr.ifr_ifindex) {
		perror("if_nametoindex");
		return 0;
	}
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    if((bind(canbus_ptr->canFd,(struct sockaddr*)&addr,sizeof(addr))) < 0)
    {
        printf("bind err!\n");
    }

    return canbus_ptr;
}


/*
canbus接收线程
canBus_t *can：用于执行线程的结构体
*/
    void CanRecvThread(canBus_t *can)
    {
        int nbytes;
        while(1)
        {
            nbytes = read(can->canFd, &can->recvBuffer, sizeof(struct can_frame)); //œÓÊÕ±šÎÄ//ÏÔÊŸ±šÎÄ
            if(nbytes > 0)
            {
                parse3(&can->recvBuffer,BO_List);

            }
        }
    }
/*
回环线程
*/
    void CanRecvLoopBack(canBus_t *can)
    {
        int nbytes;
        while(1)
        {
            nbytes = read(can->canFd, &can->recvBuffer, sizeof(struct can_frame)); //œÓÊÕ±šÎÄ//ÏÔÊŸ±šÎÄ
            if(nbytes > 0)
            {
                parse3(&can->recvBuffer,BO_List);
                int n =0;
                    while(BO_List[n] != NULL)
                    {
                        if(BO_List[n]->message_id == can->recvBuffer.can_id)
                        {
                            int m = 0;
                            while(BO_List[n]->SG_List[m] != NULL) /*SG_LOOP*/
                            {
                                BO_List[n]->SG_List[m]->value_send = BO_List[n]->SG_List[m]->value; //bypass phy value
                                m ++;
                            }
                            break;
                        }
                        n++;
                    }
            }
        }
    }
/*
can设备发送线程
*/
    void CanSendThread(taskArg_t *arg)
    {
        package_bo3(&arg->can->sendBuffer,BO_List,arg->boId);

        pthread_mutex_lock(&arg->can->rdwrLock);

        write(arg->can->canFd,&arg->can->sendBuffer,sizeof(struct can_frame));

        pthread_mutex_unlock(&arg->can->rdwrLock);

        memset(&arg->can->sendBuffer,0,sizeof(struct can_frame));
        printf("send thread done!\n");
    }

    void CanProcessThread()
    {




    }

extern void PrintTask(void);
pthread_t printThread;

/*
    采用10ms 20ms 50ms 100ms的测试程序
*/
int CanBusTestMode()
{
    timerThread = TimerRoutineInit();//初始化timer线程
    canBus_t *CAN0 = CanBusInit("can0");//初始化can设备
    canBus_t *CAN1 = CanBusInit("can1");
    pthread_t canRecvCan0;              //can0接收线程句柄
    pthread_t canRecvCan1;
    pthread_create(&canRecvCan0,NULL,(void *)CanRecvLoopBack,CAN0);//创建can0回环线程
    pthread_create(&canRecvCan1,NULL,(void *)CanRecvLoopBack,CAN1);

    pthread_create(&printThread, NULL, (void *)PrintTask, NULL);   //创建打印线程

    /*创建一个任务*/
    TaskAdd_t MASTER_CTRL_ESA_Task;//创建任务名
    MASTER_CTRL_ESA_Task.task = CanSendThread;//任务为发送线程
    MASTER_CTRL_ESA_Task.arg.boId = 1366;//获取的canbus报文为1366
    MASTER_CTRL_ESA_Task.arg.can = CAN1;//操作的设备为can1
    AddTimerTask(&MASTER_CTRL_ESA_Task, _10msTask);//添加任务，任务循环时间为10ms

    TaskAdd_t MASTER_CTRL_REQ_Task;
    MASTER_CTRL_REQ_Task.task = CanSendThread;
    MASTER_CTRL_REQ_Task.arg.boId = 1364;
    MASTER_CTRL_REQ_Task.arg.can = CAN1;
    AddTimerTask(&MASTER_CTRL_REQ_Task, _10msTask);

    TaskAdd_t SCU_STATUS_Task;
    SCU_STATUS_Task.task = CanSendThread;
    SCU_STATUS_Task.arg.boId = 1619;
    SCU_STATUS_Task.arg.can = CAN0;
    AddTimerTask(&SCU_STATUS_Task, _10msTask);

    TaskAdd_t SCU_DIAG_Task;
    SCU_DIAG_Task.task = CanSendThread;
    SCU_DIAG_Task.arg.boId = 1621;
    SCU_DIAG_Task.arg.can = CAN0;
    AddTimerTask(&SCU_DIAG_Task, _100msTask);

    TaskAdd_t SCU_FUNCTIONAL_Task;
    SCU_FUNCTIONAL_Task.task = CanSendThread;
    SCU_FUNCTIONAL_Task.arg.boId = 1620;
    SCU_FUNCTIONAL_Task.arg.can = CAN0;
    AddTimerTask(&SCU_FUNCTIONAL_Task, _10msTask);

    TaskAdd_t SCU_PREDICTION_Task;
    SCU_PREDICTION_Task.task = CanSendThread;
    SCU_PREDICTION_Task.arg.boId = 1624;
    SCU_PREDICTION_Task.arg.can = CAN0;
    AddTimerTask(&SCU_PREDICTION_Task, _10msTask);
    while(1);
    return 0;
}

/*
采用多进程的回环测试程序
*/
void PthreadCanbusTest()
{
        pthread_t showThread;
        pthread_create(&showThread, NULL, (void *)PrintTask, NULL);   //创建打印线程
        canBus_t *CAN0 = CanBusInit("can0");//初始化can设备
        canBus_t *CAN1 = CanBusInit("can1");

        pid_t pid;
        pid = fork();
        /*
        创建两个进程
        一个进程为can0收can1发
        另一个为can1收can0发
        */
        if((pid) == 0)
        {
            printf("Child thread in progress\n");
            int nbytes;
            while(1)
            {
                nbytes = read(CAN0->canFd, &CAN0->recvBuffer, sizeof(struct can_frame)); //œÓÊÕ±šÎÄ//ÏÔÊŸ±šÎÄ
                if(nbytes > 0)
                {
                    parse3(&CAN0->recvBuffer,BO_List);
                    /*loop back*/
                    int n =0;
                    while(BO_List[n] != NULL)
                    {
                        if(BO_List[n]->message_id == CAN0->recvBuffer.can_id)
                        {
                            int m = 0;
                            while(BO_List[n]->SG_List[m] != NULL) /*SG_LOOP*/
                            {
                                BO_List[n]->SG_List[m]->value_send = BO_List[n]->SG_List[m]->value; //bypass phy value
                                m ++;
                            }
                            break;
                        }
                        n++;
                    }
                    /*package*/
                    package_bo3(&CAN0->sendBuffer,BO_List,CAN0->recvBuffer.can_id);
                    /*send*/
                    nbytes = write(CAN0->canFd,&CAN0->sendBuffer,sizeof(struct can_frame));
                    memset(&CAN0->sendBuffer,0,sizeof(struct can_frame));
                }
            }
        }
        else if(pid > 0)
        {
            printf("Father thread in progress!\n");
            int nbytes;
            while(1)
            {
                nbytes = read(CAN1->canFd, &CAN1->recvBuffer, sizeof(struct can_frame)); //œÓÊÕ±šÎÄ//ÏÔÊŸ±šÎÄ
                if(nbytes > 0)
                {
                    parse3(&frame,BO_List);
                    /*loop back*/
                    int n =0;
                    while(BO_List[n] != NULL)
                    {
                        if(BO_List[n]->message_id == CAN1->recvBuffer.can_id)
                        {
                            int m = 0;
                            while(BO_List[n]->SG_List[m] != NULL) /*SG_LOOP*/
                            {
                                BO_List[n]->SG_List[m]->value_send = BO_List[n]->SG_List[m]->value; //bypass phy value
                                m ++;
                            }
                            break;
                        }
                        n++;
                    }
                    /*package*/
                    package_bo3(&CAN1->sendBuffer,BO_List,CAN1->recvBuffer.can_id);
                    /*send*/
                    nbytes = write(CAN1->canFd,&CAN1->sendBuffer,sizeof(struct can_frame));
                    memset(&CAN1->sendBuffer,0,sizeof(struct can_frame));
                }
            }
        }
}


void CanbusRecvRecord(canbuscmd_t* cmd)
{
    //canBus_t *CANTASK = CanBusInit(cmd->interface);//初始化can设备
    canBus_t *CANTASK = CanBusInit("can0");//初始化can设备
    pthread_t can_recv;
    pthread_create(&can_recv,NULL,(void *)CanRecvThread,CANTASK);//创建can0回环线程
    if(cmd->show)
    {
        pthread_t showThread;
        pthread_create(&showThread, NULL, (void *)PrintTask, NULL);   //创建打印线程
    }
    if(cmd->record)
    {

    }
    if(cmd->loopback)
    {

    }
}















