/******************************************************************
package.c
创建者：黄洪磊
时间：2018/4/23
功能：对phy信息打包成raw信息进行发送
注意：真正使用的是package_bo3，
*******************************************************************/
#include "package.h"

extern int CrossByte(int start, int size);
extern int CrossBytesLocation(int start, int size,int crossbyte);

/*
获取多少位的二进制1，mask
int nbits：多少位
返回值：mask值
*/
int bit_mask(int nbits)
{
    int i;
    int ret = 1;
    for(i = 0; i < nbits -1;i++)
    {
        ret = ret * 2+1;
    }
    return ret;
}

int raw_package_byte(int start,int size,long value,struct can_frame *frame_send)
{
    long tmp ;
    tmp = value & bit_mask(size);
    frame_send->data[start/8] |= tmp << (start%8 - size +1);
    return 1;
}

/*input canframe and canID and signal start_bit to ensure one signal in the BO_List
than pack it and |= in the frame!
*/
int package(struct can_frame *frame_send,BO_Unit_t **BO_List,unsigned int BO_ID,unsigned char SG_START,double value_send)
{
    int i = 0;
    int t = 0;
    while(BO_List[i] != NULL)
    {
        if(BO_ID == BO_List[i]->message_id)
        {
            frame_send->can_id = BO_ID;
            frame_send->can_dlc = BO_List[i]->message_size;
            /*ID matched*/
            while(BO_List[i]->SG_List[t] != NULL)
            {
                double raw_value;
                if(SG_START == BO_List[i]->SG_List[t]->start_bit)
                {
                    /*SG matched*/
                    int size = BO_List[i]->SG_List[t]->signal_size;
                    int start = BO_List[i]->SG_List[t]->start_bit;
                    /*Max and Min*/
                    if(value_send > BO_List[i]->SG_List[t]->maximum)
                        value_send = BO_List[i]->SG_List[t]->maximum;
                    if(value_send < BO_List[i]->SG_List[t]->minimum)
                        value_send = BO_List[i]->SG_List[t]->minimum;
                    /*signed and unsigned and factor_offset*/
                    if(BO_List[i]->SG_List[t]->val_type)//if signed
                    {
                        if(8 == size)
                        {
                            raw_value = (signed char)value_send;
                        }
                        else if(16 == size)
                        {
                            raw_value = value_send;
                            //printf("raw value1:%lf\n",raw_value);
                        }
                        else if(32 == size)
                        {
                            raw_value = (float)value_send;
                        }
                        /*phy 2 raw*/
                        raw_value = (raw_value - BO_List[i]->SG_List[t]->offset) / BO_List[i]->SG_List[t]->facator;
                    }
                    else
                    {
                        raw_value = (value_send - BO_List[i]->SG_List[t]->offset) / BO_List[i]->SG_List[t]->facator;
                    }
                    //printf("raw value1:%lf\n",raw_value);
                    /*package*/
                    if(size <=8)
                    {
                        raw_package_byte(start,size,raw_value,frame_send);
                    }
                    else if(size>8)
                    {
                        int location = start/8 +size/8;
                        int nbytes = size/8 +1;  //how many bytes it crosses
                        int k;
                        if(start % 8 == 7)
                        {
                            k = 0;
                            raw_package_byte((location+1)*7 + location - 8*k , size%8 ,(long)raw_value >> (8*k),frame_send);
                            for(k=1;k<nbytes;k++)
                            {
                                //frame_send->data[start/8 - k] = 0;
                                raw_package_byte( (location+1)*7 + location - 8*k ,8,(long)raw_value >> ( 8* (k-1)+(size%8) ),frame_send);
                            }
                        }
                        else if(start %8 != 7)
                        {
                            for(k=0;k<nbytes-1;k++)
                            {
                                //frame_send->data[start/8 - k] = 0;
                                raw_package_byte((location+1)*7 + location - 8*k ,8,(long)raw_value >> (8*k),frame_send);
                            }
                                raw_package_byte(start , size%8 ,(long)raw_value >> (8*k),frame_send);
                        }
                    }
                    //printf("raw value:%lf\n",raw_value);
                    //printf("value send:%lf\n",value_send);
                /*assign value_send*/
                break;
                }
                t++;
            }
            break;
        }
        i++;
    }

    return 1;
}


int package_bo(struct can_frame *frame_send,BO_Unit_t **BO_List,unsigned int BO_ID)
{
    int i = 0;
    int t = 0;
    while(BO_List[i] != NULL)
    {
        if(BO_ID == BO_List[i]->message_id)
        {
            frame_send->can_id = BO_ID;
            frame_send->can_dlc = BO_List[i]->message_size;
            /*ID matched*/
            while(BO_List[i]->SG_List[t] != NULL)
            {
                    double raw_value;
                    double value_send = BO_List[i]->SG_List[t]->value_send;
                    int size = BO_List[i]->SG_List[t]->signal_size;
                    int start = BO_List[i]->SG_List[t]->start_bit;
                    /*SG matched*/
                    /*Max and Min*/
                    if(value_send > BO_List[i]->SG_List[t]->maximum)
                        value_send = BO_List[i]->SG_List[t]->maximum;
                    if(value_send < BO_List[i]->SG_List[t]->minimum)
                        value_send = BO_List[i]->SG_List[t]->minimum;
                    /*signed and unsigned and factor_offset*/
                    if(BO_List[i]->SG_List[t]->val_type)//if signed
                    {
                        if(8 == size)
                        {
                            raw_value = value_send;
                        }
                        else if(16 == size)
                        {
                            raw_value = value_send;
                            //printf("raw value1:%lf\n",raw_value);
                        }
                        else if(32 == size)
                        {
                            raw_value = (float)value_send;
                        }
                        /*phy 2 raw*/
                        raw_value = (raw_value - BO_List[i]->SG_List[t]->offset) / BO_List[i]->SG_List[t]->facator;
                    }
                    else
                    {
                        raw_value = (value_send- BO_List[i]->SG_List[t]->offset) / BO_List[i]->SG_List[t]->facator;
                    }
                    //printf("raw value1:%lf\n",raw_value);
                    /*package*/
                    if(size <=8)
                    {
                        raw_package_byte(start,size,raw_value,frame_send);
                    }
                    else if(size>8)
                    {
                        int location = start/8 +size/8;
                        int nbytes = size/8 +1;  //how many bytes it crosses
                        int k;
                        if(start % 8 == 7)
                        {
                            k = 0;
                            raw_package_byte((location+1)*7 + location - 8*k , size%8 ,(long)raw_value >> (8*k),frame_send);
                            for(k=1;k<nbytes;k++)
                            {
                                //frame_send->data[start/8 - k] = 0;
                                raw_package_byte( (location+1)*7 + location - 8*k ,8,(long)raw_value >> ( 8* (k-1)+(size%8) ),frame_send);
                            }
                        }
                        else if(start %8 != 7)
                        {

                            for(k=0;k<nbytes-1;k++)
                            {
                                //frame_send->data[start/8 - k] = 0;
                                raw_package_byte( (location+1)*7 + location - 8*k ,8,(long)raw_value >> (8*k),frame_send);
                            }
                                raw_package_byte(start , size%8 ,(long)raw_value >> (8*k),frame_send);
                        }
                    }
                    //printf("raw value:%lf\n",raw_value);
                    //printf("value send:%lf\n",value_send);
                /*assign value_send*/


                t++;
            }
            break;
        }
        i++;
    }

    return 1;
}


int package_bo2(struct can_frame *frame_send,BO_Unit_t **BO_List,unsigned int BO_ID)
{
    int i = 0;
    int t = 0;
    while(BO_List[i] != NULL)
    {
        if(BO_ID == BO_List[i]->message_id)
        {
            frame_send->can_id = BO_ID;
            frame_send->can_dlc = BO_List[i]->message_size;
            /*ID matched*/
            while(BO_List[i]->SG_List[t] != NULL)
            {
                    double raw_value;
                    double value_send = BO_List[i]->SG_List[t]->value_send;
                    int size = BO_List[i]->SG_List[t]->signal_size;
                    int start = BO_List[i]->SG_List[t]->start_bit;
                    /*SG matched*/
                    /*Max and Min*/
                    if(value_send > BO_List[i]->SG_List[t]->maximum)
                        value_send = BO_List[i]->SG_List[t]->maximum;
                    if(value_send < BO_List[i]->SG_List[t]->minimum)
                        value_send = BO_List[i]->SG_List[t]->minimum;
                    /*signed and unsigned and factor_offset*/
                    if(BO_List[i]->SG_List[t]->val_type)//if signed
                    {
                        if(8 == size)
                        {
                            raw_value = value_send;
                        }
                        else if(16 == size)
                        {
                            raw_value = value_send;
                            //printf("raw value1:%lf\n",raw_value);
                        }
                        else if(32 == size)
                        {
                            raw_value = (float)value_send;
                        }
                        /*phy 2 raw*/
                        raw_value = (raw_value - BO_List[i]->SG_List[t]->offset) / BO_List[i]->SG_List[t]->facator;
                    }
                    else
                    {
                        raw_value = (value_send- BO_List[i]->SG_List[t]->offset) / BO_List[i]->SG_List[t]->facator;
                    }
                    //printf("raw value1:%lf\n",raw_value);
                    /*package*/
                    if(size <=8)
                    {
                        //raw_package_byte(start,size,raw_value,frame_send);
                        frame_send->data[start/8] |= ( (long)raw_value&bit_mask(size)) << (start % 8 -size +1);
                    }
                    else if(size>8)
                    {
                        if(start % 8 == 7)
                        {
                            *(frame_send->data+start/8) |= htons((long)raw_value << (8 -size %8));
                        }
                        else if(start %8 != 7)
                        {
                            *(frame_send->data+start/8) |= htons((short)raw_value );
                        }
                    }
                    printf("raw value:%lf\n",raw_value);
                    printf("value send:%lf\n",value_send);
                /*assign value_send*/
                t++;
            }
            break;
        }
        i++;
    }

    return 1;
}
/*
打包程序：将传入的BO_ID的BO_List里的结构体中的send_value打包成raw_value并放入buffer中
struct can_frame *frame_send：发送buffer
BO_Unit_t **BO_List：BO_List结构体
unsigned int BO_ID：要发送的BO_id
*/
int package_bo3(struct can_frame *frame_send,BO_Unit_t **BO_List,unsigned int BO_ID)
{
    int i = 0;
    int t = 0;
    while(BO_List[i] != NULL)
    {
        if(BO_ID == BO_List[i]->message_id)
        {
            frame_send->can_id = BO_ID;
            frame_send->can_dlc = BO_List[i]->message_size;
            /*ID matched*/
            while(BO_List[i]->SG_List[t] != NULL)
            {
                    double raw_value;
                    double value_send = BO_List[i]->SG_List[t]->value_send;
                    int size = BO_List[i]->SG_List[t]->signal_size;
                    int start = BO_List[i]->SG_List[t]->start_bit;
                    /*SG matched*/
                    /*Max and Min*/
                    if(value_send > BO_List[i]->SG_List[t]->maximum)
                        value_send = BO_List[i]->SG_List[t]->maximum;
                    if(value_send < BO_List[i]->SG_List[t]->minimum)
                        value_send = BO_List[i]->SG_List[t]->minimum;
                    /*signed and unsigned and factor_offset*/
                    if(BO_List[i]->SG_List[t]->val_type)//if signed
                    {
                        if(8 == size)
                        {
                            raw_value = value_send;
                        }
                        else if(16 == size)
                        {
                            raw_value = value_send;
                            //printf("raw value1:%lf\n",raw_value);
                        }
                        else if(32 == size)
                        {
                            raw_value = (float)value_send;
                        }
                        /*phy 2 raw*/
                        raw_value = (raw_value - BO_List[i]->SG_List[t]->offset) / BO_List[i]->SG_List[t]->facator;
                    }
                    else
                    {
                        raw_value = (value_send- BO_List[i]->SG_List[t]->offset) / BO_List[i]->SG_List[t]->facator;
                    }
                    /*package*/
                    int crossBytes = CrossByte(start, size);
                    if(crossBytes)/*cross bytes*/
                    {
                        int offset_2 = CrossBytesLocation(start, size, crossBytes);
                        if(2 == crossBytes)
                        {
                            //tmp = ((((frame->data[start/8])<<8) | (frame->data[start/8+1])) >> (offset)) & bit_mask(size);
                            //(( ((raw_value & 255) <<8) | ((raw_value & 0xff00)>>8)) & bit_mask(size)) >> offset
                            long tmple = (((long)raw_value & bit_mask(size)) <<offset_2);
                            *(short *)(frame_send->data + start/8) |= ((tmple & 255) <<8) | ((tmple & 0xff00)>>8);
                        }
                        if(3 == crossBytes)
                        {
                            //tmp =((((frame->data[start/8])<<16) | (frame->data[start/8+1]<<8)|frame->data[start/8+2]) >> (offset)) & bit_mask(size);
                            long tmple = (((long)raw_value & bit_mask(size)) <<offset_2);
                            *(long *)(frame_send->data + start/8) |= ((tmple & 255) <<16) | ((tmple & 0xff00)) | ((tmple & 0xff0000) >>16);
                        }
                    }
                    else
                    {
                        //tmp = ( (frame->data[start/8] ) >> (start%8 - size +1)) & bit_mask(size);
                        frame_send->data[start/8] |= ( (long)raw_value&bit_mask(size)) << (start % 8 -size +1);
                    }
                    /*assign value_send*/
                t++;
            }
            break;
        }
        i++;
    }

    return 1;
}







