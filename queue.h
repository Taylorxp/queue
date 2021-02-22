/***************************************************************************************************
 * FILE: queue.h
 *
 * DESCRIPTION: 
 *
 **************************************************************************************************/
#ifndef __QUEUE_H__
#define __QUEUE_H__

// #ifdef __cplusplus
// extern "C" {
// #endif

#include <pthread.h>

typedef struct byte_queue_st
{
    pthread_mutex_t mutex;
    unsigned int front;
    unsigned int rear;
    unsigned int count;
    unsigned int size;
    unsigned char *buffer;
} BYTE_QUEUE_ST, *pBYTE_QUEUE_ST;

BYTE_QUEUE_ST * ByteQueue_Init(int size);
int ByteQueue_Destroy(BYTE_QUEUE_ST **bq);
int ByteQueue_Clear(BYTE_QUEUE_ST *bq);
int ByteQueue_EnQueue(BYTE_QUEUE_ST *bq, unsigned char data);
int ByteQueue_EnQueueMutil(BYTE_QUEUE_ST *bq, unsigned char *data, int size);
int ByteQueue_DeQueue(BYTE_QUEUE_ST *bq, unsigned char *data);



// #ifdef __cplusplus
// }
// #endif

#endif
/****************************************** END OF FILE *******************************************/