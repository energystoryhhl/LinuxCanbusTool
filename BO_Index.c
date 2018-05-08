/******************************************************************
BO_Index.c
创建者：黄洪磊
时间：2018/4/23
功能：读取dbc文件并将其具体数据放入BO_List结构体数组中
*******************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "BO.h"
#include "BO_Index.h"
extern BO_Unit_t * BO_List[50];

int Free_BO_(int BO_Number)
{
    int i;
    for(i = BO_Number;i>0;i--)
    {
        free(BO_List[BO_Number]);
    }
    return 0;
}

/*  Detect_BO
    Just for one line!
    在dbc文件中检测BO_行
*/
char * Detect_BO(FILE * fptr,char * rdbuf)
{

    fgets(rdbuf,100,fptr);//READ THE FILE =>One Line!
    if(0 == (strncmp(rdbuf,BOFLAG,4)))
    {
            //printf("%s",rdbuf); //FOR SHOW
            return(rdbuf);
    }
    return NULL;
}

/*Parse the SG_ content
rdbuf : the buffer of readed
num: the number of SG_ index
解析SG_行
rdbuf：读取dbc文件时 一行的buffer
num：SG_行中参数的数量
*/
SG_t* Parse_Line3(const char *rdbuf,int num)
{
    SG_t *sg_ptr = malloc(sizeof(SG_t));

    int flag[num][2];
    int flag_qian= 0;
    int flag_hou = -1;
    int flag_i = 0;
    int i = 0;
    int times = 0;
    int stop_flag = 1;
    while(stop_flag)
    {
        while(rdbuf[i] == ' ')
        {
            //printf("%c",rdbuf[i]);
            i++;
        }
        flag_qian = i-1;
        while((rdbuf[i] != ' '))
        {
            //printf("%c",rdbuf[i]);
            if(rdbuf[i] == '\n')
            {
                stop_flag = 0;
                break;
            }
            i++;
        }
        times++;
        flag_hou = i;
        flag[flag_i][0] =  flag_qian+1;
        flag[flag_i][1] =  flag_hou-1;
        #ifdef DEBUGLINE
        printf("   %d --> %d  time:%d \n",flag_qian+1,flag_hou-1,times);
        #endif // DEBUGLINE

        flag_i++;
    }

    char tmp[50];

    /*SG_name*/
    memset(tmp,0,50);
    strncpy(tmp,(rdbuf+flag[1][0]), (flag[1][1] - flag[1][0] +1));
    //boptr->message_name[flag[2][1] - flag[2][0] +1] = '\0';
    tmp[flag[1][1] - flag[1][0]+1] = '\0';
    strncpy(sg_ptr->signal_name,tmp,strlen(tmp));
    #ifdef DEBUGPARSE
    printf("/*SG_name*/: %s\n",sg_ptr->signal_name);
    #endif // DEBUGPARSE
    /*Start Size Order Type*/
    memset(tmp,0,50);
    strncpy(tmp,(rdbuf+flag[3][0]), (flag[3][1] - flag[3][0] +1));
    #ifdef DEBUGPARSE
    printf("/*Start_Size*/%s ",tmp);
    #endif // DEBUGPARSE
    /*Get start bit*/
    char tmp_start_size[30];
    int n = 0;
    int m = 0;
    while(tmp[n] != '|')
    {
        tmp_start_size[m] = tmp[n];
        n++;
        m++;
    }
    tmp_start_size[m+1] = '\0';
    sg_ptr->start_bit = atoi(tmp_start_size);
    #ifdef DEBUGPARSE
    printf("start>>>%d ",sg_ptr->start_bit);
    #endif // DEBUGPARSE

    /*Get Size*/
    n++;
    memset(tmp_start_size,0,30);
    m = 0;
    while(tmp[n] != '@')
    {
        tmp_start_size[m] = tmp[n];
        n++;
        m++;
    }
    tmp_start_size[m+1] = '\0';
    sg_ptr->signal_size = atoi(tmp_start_size);
    #ifdef DEBUGPARSE
    printf("size>>>%d ",sg_ptr->signal_size);
    #endif // DEBUGPARSE

    /*Get Bit_order*/
    n++;
    memset(tmp_start_size,0,30);
    m = 0;
    if(tmp[n] == '0')
    {
        sg_ptr->Bit_order = motorola;
    }
    else if(tmp[n] == '1')
    {
        sg_ptr->Bit_order = intel;
    }
    else
    {
        printf("BIT ORDER GET ERROR!\n");
        return NULL;
    }
    #ifdef DEBUGPARSE
    printf("order>>>%d ",sg_ptr->Bit_order);
    #endif // DEBUGPARSE

    /*Get Bit Type*/
    n++;
    if(tmp[n] == '+')
    {
        sg_ptr->val_type = unsigned_value;
    }
    else if(tmp[n] == '-')
    {
        sg_ptr->val_type = signed_value;
    }
    else
    {
        printf("BIT TYPE GET ERROR!");
        return NULL;
    }
    #ifdef DEBUGPARSE
    printf("type>>>%d \n",sg_ptr->val_type);
    #endif // DEBUGPARSE

    //clear
    n = 0, m = 0;

    /*Get factor and offset*/
    n = 1;
    memset(tmp,0,50);
    memset(tmp_start_size,0,30);
    strncpy(tmp,(rdbuf+flag[4][0]), (flag[4][1] - flag[4][0] +1));
    //#ifdef DEBUGPARSE
    #ifdef DEBUGPARSE
    printf("/*factor*/%s ",tmp);
    #endif // DEBUGPARSE
    //#endif // DEBUGPARSE
    /*Get factor*/
    while(tmp[n] != ',')
    {
        tmp_start_size[m] = tmp[n];
        n++;
        m++;
    }
    tmp_start_size[m+1] = '\0';
    sg_ptr->facator = atof(tmp_start_size);
    #ifdef DEBUGPARSE
    printf("factor>>>%lf ",sg_ptr->facator);
    #endif // DEBUGPARSE

    /*Get offset*/
    n++;
    m = 0;
    memset(tmp_start_size,0,30);
    while(tmp[n] != ')')
    {
        tmp_start_size[m] = tmp[n];
        m++;
        n++;
    }
    tmp_start_size[m+1] = '\0';
    sg_ptr->offset = atof(tmp_start_size);
    #ifdef DEBUGPARSE
    printf("offset>>>%lf \n",sg_ptr->offset);
    #endif // DEBUGPARSE

    n = 0, m = 0;
    memset(tmp,0,50);
    memset(tmp_start_size,0,30);

    /*Get Max & Min*/
    strncpy(tmp,(rdbuf+flag[5][0]), (flag[5][1] - flag[5][0] +1));
    //#ifdef DEBUGPARSE
    #ifdef DEBUGPARSE
    printf("/*Max&Min*/%s ",tmp);
    #endif
    n = 1;
    while(tmp[n] != '|')
    {
        tmp_start_size[m] = tmp[n];
        n++;
        m++;
    }
    tmp_start_size[m+1] = '\0';
    sg_ptr->minimum = atof(tmp_start_size);
    #ifdef DEBUGPARSE
    printf("Min>>>%lf ",sg_ptr->minimum);
    #endif // DEBUGPARSE
    /*MAX*/
    n++;
    m = 0;
    memset(tmp_start_size,0,30);
    while(tmp[n] != ']')
    {
        tmp_start_size[m] = tmp[n];
        n++;
        m++;
    }
    tmp_start_size[m+1] = '\0';
    sg_ptr->maximum = atof(tmp_start_size);
    #ifdef DEBUGPARSE
    printf("Max>>>%lf \n",sg_ptr->maximum);
    #endif // DEBUGPARSE
    n = 0, m = 0;
    memset(tmp,0,50);
    memset(tmp_start_size,0,30);

    /*Get Unit*/
    strncpy(tmp,(rdbuf+flag[6][0]), (flag[6][1] - flag[6][0] +1));
    //#ifdef DEBUGPARSE
    strncpy(sg_ptr->unit,tmp+1,strlen(tmp)-2);
    #ifdef DEBUGPARSE
    printf("Unit: %s \n",sg_ptr->unit);
    #endif // DEBUGPARSE
    n = 0, m = 0;
    memset(tmp,0,50);
    memset(tmp_start_size,0,30);

    /*Get receiver*/
    strncpy(tmp,(rdbuf+flag[7][0]), (flag[7][1] - flag[7][0] +1));
    //#ifdef DEBUGPARSE
    strncpy(sg_ptr->receiver,tmp,strlen(tmp)-1);
    #ifdef DEBUGPARSE
    printf("receiver: %s \n",sg_ptr->receiver);
    printf("--------------******--------------\n");
    #endif // DEBUGPARSE
    n = 0, m = 0;
    memset(tmp,0,50);
    memset(tmp_start_size,0,30);

    return sg_ptr;
}

//BO_Unit_t *boptr
/*bo is readbuffer
获取BO_行具体参数
const char * bo：为BO_行的读取buffer
BO_Unit_t *boptr：为这个BO_结构体的指针
*/
int Get_BO_Index(const char * bo,BO_Unit_t *boptr)
{
    int flag[5][2];
    int flag_qian= 0;
    int flag_hou = -1;
    int i = 0;
    int flag_i = 0;
    int times = 0;
     while(bo[i] != '\n')
     {

        if(bo[i] == ' ')
        {
            flag_qian = flag_hou;
            flag_hou = i;
            times++;
            #ifdef DEBUG
            //printf("   %d --> %d  time:%d \n",flag_qian+1,flag_hou-1,times);
            #endif // DEBUG
            flag[flag_i][0] =  flag_qian+1;
            flag[flag_i][1] =   flag_hou-1;
            flag_i++;
        }
        //printf("%c",bo[i]);
        i++;

        if(bo[i] == '\n')
        {
            flag_qian = flag_hou;
            flag_hou = i;
            times++;
            #ifdef DEBUG
            //printf("   %d --> %d  time:%d \n",flag_qian+1,flag_hou-1,times);
            #endif // DEBUG
            flag[flag_i][0] =  flag_qian+1;
            flag[flag_i][1] =   flag_hou-1;
            flag_i++;
        }
     }
    #ifdef DEBUG
//    for(int t = 0;t<5;t++)
//    {
//        printf("%d",flag[t][0]);
//        printf("--->");
//        printf("%d",flag[t][1]);
//        printf("\n");
//    }
    #endif // DEBUG

    /*msg id:*/
    char tmp[50];
    strncpy((char *)tmp,(const char *)(bo+flag[1][0]), (int)(flag[1][1] - flag[1][0] +1));
    tmp[flag[1][1] - flag[1][0] +1] = '\0';
    //for show
    #ifdef DEBUG
//    printf("tmp is %s \n",tmp);
    #endif // DEBUG
    boptr->message_id = atoi(tmp);
    #ifdef DEBUG
    printf(" id is: %u \t",boptr ->message_id);
    #endif // DEBUG

    /*message_name*/
    memset(tmp,0,50);
    strncpy(tmp,(bo+flag[2][0]), (flag[2][1] - flag[2][0] +1));
    //boptr->message_name[flag[2][1] - flag[2][0] +1] = '\0';
    tmp[flag[2][1] - flag[2][0]] = '\0';

    strcpy(boptr->message_name,tmp);
    //printf(">>>>>%s\n",boptr->message_name);

    #ifdef DEBUG
    printf("Msg name: %s \t",boptr->message_name);
    #endif // DEBUG

    /*DLC*/
    memset(tmp,0,50);
    strncpy((char *)tmp,(const char *)(bo+flag[3][0]), (int)(flag[3][1] - flag[3][0] +1));
    tmp[flag[3][1] - flag[3][0] +1] = '\0';
    #ifdef DEBUG
//    printf("\ntmp is %s \n",tmp);
    #endif // DEBUG
    boptr->message_size = atoi(tmp);
    #ifdef DEBUG
    printf("DLC: %d \t",boptr ->message_size);
    #endif // DEBUG

    /*message_name*/
    memset(tmp,0,50);
    strncpy((char *)boptr->transmitter,(const char *)(bo+flag[4][0]), (int)(flag[4][1] - flag[4][0] +1));
    boptr->transmitter[flag[4][1] - flag[4][0] +1] = '\0';
    #ifdef DEBUG
    printf("Transmitter: %s \n",boptr->transmitter);
    #endif

    /*SG_t init*/
    boptr->First_SG =NULL;

    return 1;
}


/*
获取SG_行的具体信息函数
FILE * fptr：dbc文件句柄
char * rdbuf：读取一行的buffer
BO_Unit_t *boptr：存放信息的结构体指针
*/
int Get_SG_Index(FILE * fptr,char * rdbuf,BO_Unit_t *boptr)
{
    int i = 0;
    while(1)
    {
        fgets(rdbuf,100,fptr);

        if(0 == (strncmp(rdbuf,SGFLAG,5))) //DETECT THE SG_ while the end of THE hole SG_ of one BO_
        {
            /*OPERATION*/
            /*Parse the content of SG and save in the SG_T List*/
            SG_t *tmpsg = Parse_Line3(rdbuf,8);
           // printf("SG_Name:%s  start_bit:%d  size:%d  byte_order:%d  value_type:%d\nfactor:%lf offset:%lf Min:%lf Max:%lf unit:%s receiver:%s \n",
           // sgptr->signal_name,sgptr->start_bit,sgptr->signal_size,sgptr->Bit_order,sgptr->val_type,sgptr->facator,sgptr->offset,sgptr->minimum,sgptr->maximum,sgptr->unit,sgptr->receiver);
            //printf(">>>offset %lf \n",tmpsg->offset);
            /*Let the SG in to the SG_List*/
            boptr->SG_List[i] = tmpsg;
            i++;
//            SG_t *tmp_ptr = boptr->First_SG;
//            if(tmp_ptr!=NULL)
//            {
//                while(tmp_ptr->next !=NULL)
//                {
//                    tmp_ptr = tmp_ptr->next;
//                }
//                tmp_ptr->next = tmpsg;
//            }
//            else
//            {
//                tmp_ptr = tmpsg;
//            }

        }
        else
        {
            //printf("+++++++++++++++++++++++\n");
            break;
        }
    }
    return 0;
}

/*dbc文件读取函数
FILE * dbc_cfg_fd ： dbc文件句柄
char *dbc_cfg_rdbuf：读取dbc文件时所需的buffer
BO_Unit_t **BO_List：存放dbc中读取到的信息的结构体指针数组
*/
int Dbc_load(FILE * dbc_cfg_fd,char *dbc_cfg_rdbuf,BO_Unit_t **BO_List)
{
    int BO_counter = 0;
    /*Read dbc file*/
    while(1)
    {
        /*detect the BO_ */
        if( Detect_BO(dbc_cfg_fd,dbc_cfg_rdbuf) !=NULL )
        {
            /*OPERATION*/
            BO_List[BO_counter] =  (BO_Unit_t *)malloc(sizeof(BO_Unit_t));
            memset(BO_List[BO_counter],0,sizeof(BO_Unit_t));
            Get_BO_Index(dbc_cfg_rdbuf,BO_List[BO_counter]);
            /*detect the SG_ and parse*/
            Get_SG_Index(dbc_cfg_fd,dbc_cfg_rdbuf,BO_List[BO_counter]);
            BO_counter++;
        }
        if(feof(dbc_cfg_fd))                    //IF THE END OF FILE
        {break;}
    }

    /*SHOW dec file content*/
    int y;
    for(y = 0;y<BO_counter;y++)
    {
        int i = 0;
        printf("  ID:%u  Name:%s  DLC:%d Transmitter:%s \n",
                        BO_List[y]->message_id,
                        BO_List[y]->message_name,
                        BO_List[y]->message_size,
                        BO_List[y]->transmitter);

        while(BO_List[y]->SG_List[i] != NULL)
        {
            //printf(">>factor:%lf\n",BO_List[y]->SG_List[i]->facator);
            printf("-------------------SG INF--------------------------\n");
            printf("SG_Name:%s  start_bit:%d  size:%d  byte_order:%d  value_type:%d\n factor:%lf offset:%lf Min:%lf Max:%lf unit:%s receiver:%s \n",
            BO_List[y]->SG_List[i]->signal_name,BO_List[y]->SG_List[i]->start_bit,BO_List[y]->SG_List[i]->signal_size,BO_List[y]->SG_List[i]->Bit_order,
            BO_List[y]->SG_List[i]->val_type,BO_List[y]->SG_List[i]->facator,BO_List[y]->SG_List[i]->offset,BO_List[y]->SG_List[i]->minimum,
            BO_List[y]->SG_List[i]->maximum,BO_List[y]->SG_List[i]->unit,BO_List[y]->SG_List[i]->receiver);
            i++;
        }
//                while(sgptr->next != NULL)
//                {
//                    printf("SG_Name:%s  start_bit:%d  size:%d  byte_order:%d  value_type:%d\n factor:%lf offset:%lf Min:%lf Max:%lf unit:%s receiver:%s",
//                    sgptr->signal_name,sgptr->start_bit,sgptr->signal_size,sgptr->Bit_order,sgptr->val_type,sgptr->facator,sgptr->offset,sgptr->minimum,sgptr->unit,sgptr->receiver);
//
//                    sgptr = sgptr->next;
//                }
        printf("\n=======================BO INF========================================\n");
    }

    return BO_counter;
}

//num: the number of elements
//int Parse_Line(const char *rdbuf,int num)
//{
//    int flag[num][2];
//    int flag_qian= 0;
//    int flag_hou = -1;
//    int flag_i = 0;
//    int i = 0;
//    int times = 0;
//    while(rdbuf[i] != '\n')
//    {
//        if(rdbuf[i] == ' ')
//        {
//            flag_qian = flag_hou;
//            flag_hou = i;
//            times++;
//            #ifdef DEBUGLINE
//            printf("   %d --> %d  time:%d \n",flag_qian+1,flag_hou-1,times);
//            #endif // DEBUG
//            flag[flag_i][0] =  flag_qian+1;
//            flag[flag_i][1] =   flag_hou-1;
//            flag_i++;
//        }
//        printf("%c",rdbuf[i]);
//        i++;
//
//        if(rdbuf[i] == '\n')
//        {
//            flag_qian = flag_hou;
//            flag_hou = i;
//            times++;
//            #ifdef DEBUGLINE
//            printf("   %d --> %d  time:%d \n",flag_qian+1,flag_hou-1,times);
//            #endif // DEBUG
//            flag[flag_i][0] =  flag_qian+1;
//            flag[flag_i][1] =   flag_hou-1;
//            flag_i++;
//        }
//
//    }
//
//    return 1;
//}

//
//int Parse_Line2(const char *rdbuf,int num)
//{
//    int flag[num][2];
//    int flag_qian= 0;
//    int flag_hou = -1;
//    int flag_i = 0;
//    int i = 0;
//    int times = 0;
//    int stop_flag = 1;
//    while(stop_flag)
//    {
//        while(rdbuf[i] == ' ')
//        {
//            //printf("%c",rdbuf[i]);
//            i++;
//        }
//        flag_qian = i-1;
//        while((rdbuf[i] != ' '))
//        {
//            printf("%c",rdbuf[i]);
//            if(rdbuf[i] == '\n')
//            {
//                stop_flag = 0;
//                break;
//            }
//            i++;
//        }
//        times++;
//        flag_hou = i;
//        flag[flag_i][0] =  flag_qian+1;
//        flag[flag_i][1] =  flag_hou-1;
//
//        printf("   %d --> %d  time:%d \n",flag_qian+1,flag_hou-1,times);
//        flag_i++;
//    }
//    return 1;
//}



/******************************************************************


*******************************************************************/






