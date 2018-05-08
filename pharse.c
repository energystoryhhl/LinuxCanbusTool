/******************************************************************
parse.c
创建者：黄洪磊
时间：2018/4/23
功能：将raw信息进行解析成phy信息
注意：真正使用的是parse3
*******************************************************************/


#include "pharse.h"

extern int bit_mask(int nbits);
/*for bit <=8*/
char bit_get(const int start,const int size,const char target)
{
    int s = start;
    if(s > 8)
    {
        s = s%8;
    }
    char tmp = 0;
    int z = size;
    char t = target;
    while(z>0)
    {
        tmp |= 1<<s;
        s--;
        z--;
    }
    t = (t&tmp) >> (s - z +1);
    return t;
}

/*for bit >8*/                    /*unsigned char? char?*/
long nbyte_get(int start,int size, unsigned char *target)
{
    int nbytes = size/8 +1;
    //double ret = 0;
    char tmp[8];
    long ret = 0;
    if(start%8 != 7)
    {
        tmp[0] = bit_get(start,start%8+1,target[start/8]);
        int i = 1;
        while(nbytes>0)
        {
            tmp[i] = target[start/8 + i];
            nbytes--;
            i++;
        }
        int t = 0;
        for(i = size/8 ;i>=0;i--)
        {
            ret = ret |(tmp[i] << (8*t));
            //printf("i is %d ret is %ld\n",i,ret);
            t++;
        }
        //printf("ret is :%ld\n",ret);
    }
    else if(start%8 == 7)
    {
        int i = 0;
        while(nbytes > 1)
        {
            tmp[i] = target[start/8 + i];
            i++;
            nbytes --;
        }
        tmp[i] = bit_get(start,size%8,target[start/8+i]);
        int t = 0;
        for(i = size/8 ;i>=0;i--)
        {
            if(i == size/8)
            {
                ret = ret |tmp[size/8];
            }
            else
            {
                ret = ret |(tmp[i] << (8*t + (size%8)));
                t++;
            }
            //printf("i is %d ret is %ld\n",i,ret);
        }
        //printf("ret is :%ld\n",ret);
    }
    return ret;
}

//extern BO_Unit_t * BO_List[50];
void pharse(struct can_frame *frame,BO_Unit_t **BO_List)
{
    int i = 0;
    while(BO_List[i] != NULL)
    {
        if(frame->can_id == BO_List[i]->message_id) //detect the can id
        {
            /*ID Mached*/
            int t = 0;
            while(BO_List[i]->SG_List[t] != NULL) /*SG_LOOP*/
            {
                /*motorola Mode*/
                if(0 == BO_List[i]->SG_List[t]->Bit_order)
                {
                    int size = BO_List[i]->SG_List[t]->signal_size;
                    int start = BO_List[i]->SG_List[t]->start_bit;
                    double tmp = 0;
                    /*Pharse the motorla mode msg!*/
                    if(size <= 8)
                    {
                        tmp = (double)bit_get(start,size,frame->data[start/8]);
                    }
                    else if(size > 8)
                    {
                        tmp = (double)nbyte_get(start,size, frame->data);
                    }
                    /*value type_ signed or unsigned*/
                    if(BO_List[i]->SG_List[t]->val_type)
                    {
                        if(8 == size)
                        {
                            tmp = (signed char)tmp;
                            tmp = (double)tmp;
                        }
                        else if(16 == size)
                        {
                            tmp = (short)tmp;
                            //tmp = (double)tmp;
                        }
                        else if(32 == size)
                        {
                            tmp = (float)tmp;
                            //tmp = (double)tmp;
                        }
                    }
                    /*Factor and Offset*/
                        tmp = tmp * BO_List[i]->SG_List[t]->facator + BO_List[i]->SG_List[t]->offset;
                    /*Max and Min*/
                        if(tmp > BO_List[i]->SG_List[t]->maximum)
                        {
                            tmp = BO_List[i]->SG_List[t]->maximum;
                        }
                        if(tmp < BO_List[i]->SG_List[t]->minimum)
                        {
                            tmp = BO_List[i]->SG_List[t]->minimum;
                        }
                    /*assign value*/
                    BO_List[i]->SG_List[t]->value = tmp;
                }
                t++;
            }
        break;
        }
        i++;
    }
    //printf("Number of BO:%d\n",i);
}





void pharse2(struct can_frame *frame,BO_Unit_t **BO_List)
{
    int i = 0;
    while(BO_List[i] != NULL)
    {
        //printf("bolist loop \n ");
        if(frame->can_id == BO_List[i]->message_id) //detect the can id
        {
            /*ID Mached*/
            #ifdef SHOW
            system("clear");
            printf("BO_ %d %s\n",BO_List[i]->message_id,BO_List[i]->message_name);
            #endif // SHOW
            //printf("\r");
            int t = 0;
            while(BO_List[i]->SG_List[t] != NULL) /*SG_LOOP*/
            {
                #ifdef SHOW
                printf("%s ",BO_List[i]->SG_List[t]->signal_name);
                #endif // SHOW
                /*motorola Mode*/
                if(0 == BO_List[i]->SG_List[t]->Bit_order)
                {
                    int size = BO_List[i]->SG_List[t]->signal_size;
                    int start = BO_List[i]->SG_List[t]->start_bit;
                    double tmp = 0;
                    /*Pharse the motorla mode msg!*/
                    //bite < 8
                    if(size <= 8)
                    {
                        tmp = ( (frame->data[start/8] ) >> (start%8 - size +1)) & bit_mask(size);
//                        printf("%lf\r",tmp);
                    }
                    else if(size > 8 && size <= 16)
                    {
                        //printf("==\r");
                        if(start / 8 == 7)
                        {
                            tmp = ((short)(frame->data[start/8 ]<<8 | frame->data[start/8 + 1]) >> (size % 8))& bit_mask(size);
                        }
                            tmp = (short)(frame->data[start/8 ]<<8 | frame->data[start/8 + 1]) & bit_mask(size);
//                        printf(">>%lf\r",tmp);
                    }
                    /*value type_ signed or unsigned*/
                    else if(size >16)
                    {
                        tmp = ntohl(*(long *)(frame->data+(start/8))) & bit_mask(size);
                    }
                    if(BO_List[i]->SG_List[t]->val_type)
                    {
                        if(8 == size)
                        {
                            tmp = (signed char)tmp;
                            tmp = (double)tmp;
                        }
                        else if(16 == size)
                        {
                            tmp = (short)tmp;
                            //tmp = (double)tmp;
                        }
                        else if(32 == size)
                        {
                            tmp = (float)tmp;
                            //tmp = (double)tmp;
                        }
                    }
                    /*Factor and Offset*/
                        tmp = tmp * BO_List[i]->SG_List[t]->facator + BO_List[i]->SG_List[t]->offset;
                    /*Max and Min*/
                        if(tmp > BO_List[i]->SG_List[t]->maximum)
                        {
                            tmp = BO_List[i]->SG_List[t]->maximum;
                        }
                        if(tmp < BO_List[i]->SG_List[t]->minimum)
                        {
                            tmp = BO_List[i]->SG_List[t]->minimum;
                        }
                    /*assign value*/
                    BO_List[i]->SG_List[t]->value = tmp;
                    /*Print nuit*/
                    #ifdef SHOW
                    printf(">>%lf ",tmp);
                    printf(" %s ", BO_List[i]->SG_List[t]->unit);
                    #endif // SHOW
                }
                #ifdef SHOW
                printf("\n");
                #endif // SHOW
                t++;
            }
        break;
        }
        i++;
    }
    //printf("Number of BO:%d\n",i);
}
    //if the signal cross bytes
int CrossByte(int start, int size)
{
    int tmp = (start % 8)+1 -size;

    if( tmp < 0 )
    {
        if(tmp < -16)
        {
            return 4;
        }
        else if(tmp < -8)
        {
            return 3;
        }
        else
            return 2;
    }
    return 0;
}

int CrossBytesLocation(int start, int size,int crossbyte)
{
    int offset = 0;
    if(crossbyte == 2)
        offset = 8;
    else if(crossbyte == 3)
        offset = 16;
    else if(crossbyte == 4)
        offset = 24;
    int ret = 1 + offset -(size - (start % 8));
    return ret ;
}
/*
raw信息解析函数，将frame中的raw信息解析为phy信息
放入BO_List中的value中
struct can_frame *frame：要解析的frame
BO_Unit_t **BO_List：BO_List结构体
*/
void parse3(struct can_frame *frame,BO_Unit_t **BO_List)
{
    int i = 0;
    while(BO_List[i] != NULL)
    {
        //printf("bolist loop \n ");
        if(frame->can_id == BO_List[i]->message_id) //detect the can id
        {
            /*ID Mached*/
            int t = 0;
            while(BO_List[i]->SG_List[t] != NULL) /*SG_LOOP*/
            {
                if(0 == BO_List[i]->SG_List[t]->Bit_order)
                {
                    int size = BO_List[i]->SG_List[t]->signal_size;
                    int start = BO_List[i]->SG_List[t]->start_bit;
                    double tmp = 0;
                    /*Pharse the motorla mode msg!*/
                    int crossBytes = CrossByte(start, size);
                    if(crossBytes)/*cross bytes*/
                    {
                        int offset = CrossBytesLocation(start, size, crossBytes);
                        if(2 == crossBytes)
                        {
                            tmp = ((((frame->data[start/8])<<8) | (frame->data[start/8+1])) >> (offset)) & bit_mask(size);
                        }
                        if(3 == crossBytes)
                        {
                            tmp =((((frame->data[start/8])<<16) | (frame->data[start/8+1]<<8)|frame->data[start/8+2]) >> (offset)) & bit_mask(size);
                        }
                    }
                    else
                    {
                        tmp = ( (frame->data[start/8] ) >> (start%8 - size +1)) & bit_mask(size);
                    }

                    /*value type_ signed or unsigned*/
                    if(BO_List[i]->SG_List[t]->val_type)
                    {
                        if(8 == size)
                        {
                            tmp = (signed char)tmp;
                            tmp = (double)tmp;
                        }
                        else if(16 == size)
                        {
                            tmp = (short)tmp;
                            //tmp = (double)tmp;
                        }
                        else if(32 == size)
                        {
                            tmp = (float)tmp;
                            //tmp = (double)tmp;
                        }
                    }
                    /*Factor and Offset*/
                        tmp = tmp * BO_List[i]->SG_List[t]->facator + BO_List[i]->SG_List[t]->offset;
                    /*Max and Min*/
                        if(tmp > BO_List[i]->SG_List[t]->maximum)
                        {
                            tmp = BO_List[i]->SG_List[t]->maximum;
                        }
                        if(tmp < BO_List[i]->SG_List[t]->minimum)
                        {
                            tmp = BO_List[i]->SG_List[t]->minimum;
                        }
                    /*assign value*/
                    BO_List[i]->SG_List[t]->value = tmp;
                    /*Print nuit*/
                    #ifdef SHOW
                    printf(">>%lf ",tmp);
                    printf(" %s ", BO_List[i]->SG_List[t]->unit);
                    #endif // SHOW
                }
                #ifdef SHOW
                printf("\n");
                #endif // SHOW
                t++;
            }
        break;
        }
        i++;
    }
    //printf("Number of BO:%d\n",i);
}
