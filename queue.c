/***************************************************************************************************
 * FILE: queue.c
 *
 * DESCRIPTION: 字节队列
 *
 **************************************************************************************************/
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <pthread.h>

#include "log.h"
#include "queue.h"

/***************************************************************************************************
 * Description: 打印队列基本信息
 **************************************************************************************************/
void ByteQueue_Dump(BYTE_QUEUE_ST *bq)
{
    if (bq == NULL || bq->buffer == NULL)
    {
        LOG_ERROR("null ptr!");
        return;
    }

    printf("=== ByteQueue Dump ===\nSize:\t%u\nCount:\t%u\nFront:\t%u\nRear:\t%u\nBuffer:\t0x%08x\n",
           bq->size, bq->count, bq->front, bq->rear, (unsigned int)bq->buffer);
    int i;
    for (i = 0; i < 256; i++)
    {
        printf("%02x ", bq->buffer[i]);
        if (((i + 1) % 16) == 0)
            putchar('\n');
    }
    putchar('\n');
}

/***************************************************************************************************
 * Description: 初始化字节队列
 **************************************************************************************************/
BYTE_QUEUE_ST *ByteQueue_Init(int size)
{
    BYTE_QUEUE_ST *bq;

    bq = malloc(sizeof(BYTE_QUEUE_ST));
    if (bq == NULL)
    {
        LOG_ERROR("byte queue malloc failure!");
        return NULL;
    }
    memset(bq, 0, sizeof(BYTE_QUEUE_ST));

    bq->buffer = (unsigned char *)malloc(size);
    if (bq->buffer == NULL)
    {
        free(bq);
        LOG_ERROR("byte queue buffer malloc failure!");
        return NULL;
    }
    bq->size = size;
    memset(bq->buffer, 0, size);

    return bq;
}

/***************************************************************************************************
 * Description: 销毁字节队列
 **************************************************************************************************/
int ByteQueue_Destroy(BYTE_QUEUE_ST **bq)
{
    if (*bq == NULL || (*bq)->buffer == NULL)
    {
        LOG_ERROR("null ptr!");
        return -1;
    }

    free((*bq)->buffer);
    free(*bq);
    *bq = NULL;
    return 0;
}

/***************************************************************************************************
 * Description: 字节队列清空
 **************************************************************************************************/
int ByteQueue_Clear(BYTE_QUEUE_ST *bq)
{
    pthread_mutex_lock(&(bq->mutex));

    while (bq->count)
    {
        bq->count--;
        if (++bq->rear >= bq->size)
        {
            bq->rear = 0;
        }
    }

    pthread_mutex_unlock(&(bq->mutex));
    return 0;
}

/***************************************************************************************************
 * Description: 字节队列入队
 **************************************************************************************************/
int ByteQueue_EnQueue(BYTE_QUEUE_ST *bq, unsigned char data)
{
    if (bq == NULL || bq->buffer == NULL)
    {
        LOG_ERROR("null ptr!");
        return -1;
    }

    pthread_mutex_lock(&(bq->mutex));

    bq->buffer[bq->front] = data;

    if (++bq->front >= bq->size)
    {
        bq->front = 0;
    }

    if (++bq->count > bq->size)
    {
        bq->count = bq->size;
        if (++bq->rear >= bq->size)
        {
            bq->rear = 0;
        }
    }
    // ByteQueue_Dump(bq);
    pthread_mutex_unlock(&(bq->mutex));
    return 0;
}

/***************************************************************************************************
 * Description: 字节队列多元素入队
 **************************************************************************************************/
int ByteQueue_EnQueueMutil(BYTE_QUEUE_ST *bq, unsigned char *data, int size)
{
    if (bq == NULL || bq->buffer == NULL)
    {
        LOG_ERROR("null ptr!");
        return -1;
    }

    if (data == NULL || size <= 0 || size > bq->size)
    {
        LOG_ERROR("invalid parameter!");
        return -EINVAL;
    }

    pthread_mutex_lock(&(bq->mutex));

    /* 效率待优化 */
    int i;
    for (i = 0; i < size; i++)
    {
        bq->buffer[bq->front] = data[i];

        if (++bq->front >= bq->size)
        {
            bq->front = 0;
        }

        if (++bq->count > bq->size)
        {
            bq->count = bq->size;
            if (++bq->rear >= bq->size)
            {
                bq->rear = 0;
            }
        }
    }

    // ByteQueue_Dump(bq);
    // PRT_LOG("Enqueue\n");
    pthread_mutex_unlock(&(bq->mutex));
    return 0;
}

/***************************************************************************************************
 * Description: 字节队列出对
 **************************************************************************************************/
int ByteQueue_DeQueue(BYTE_QUEUE_ST *bq, unsigned char *data)
{
    if (bq == NULL || bq->buffer == NULL)
    {
        LOG_ERROR("null ptr!");
        return -1;
    }

    pthread_mutex_lock(&(bq->mutex));

    if (bq->count == 0)
    {
        pthread_mutex_unlock(&(bq->mutex));
        return 1;
    }

    *data = bq->buffer[bq->rear];
    bq->count--;
    if (++bq->rear >= bq->size)
    {
        bq->rear = 0;
    }
    // ByteQueue_Dump(bq);
    // PRT_LOG("Dequeue\n");
    pthread_mutex_unlock(&(bq->mutex));
    return 0;
}

/****************************************** END OF FILE *******************************************/
