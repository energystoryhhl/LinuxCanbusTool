#ifndef __PACKAGE_H__
#define __PACKAGE_H__
#include "main.h"
#include "BO.h"
int package(struct can_frame *frame_send,BO_Unit_t **BO_List,unsigned int BO_ID,unsigned char SG_START,double value_send);

int package_bo(struct can_frame *frame_send,BO_Unit_t **BO_List,unsigned int BO_ID);

int package_bo2(struct can_frame *frame_send,BO_Unit_t **BO_List,unsigned int BO_ID);

int package_bo3(struct can_frame *frame_send,BO_Unit_t **BO_List,unsigned int BO_ID);

#endif // __PACKAGE_H__
