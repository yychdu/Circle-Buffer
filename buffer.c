#define _CRT_SECURE_NO_WARNINGS 1
#define _CRT_NONSTDC_NO_DEPRECATE 1
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "buffer.h"
#define validLen Object->push_size - Object->pop_size    //缓冲区有效长度

TBufffer TBuffer_Init(TBuffferData *data, void *buffer, int buff_size)
{
    TBufffer result = NULL;
    if (data != NULL && buffer != NULL && buff_size > 0)
    {
        data->pHead = buffer;
        data->size = buff_size;
        data->pTail = data->pHead + buff_size;
        data->pValid = data->pHead;
        data->pValidTail = data->pHead;
        data->push_size = 0;
        data->pop_size = 0;
        result = data;
    }
    return result;
}

void TBuffer_Fini(TBuffferData *data)
{
    if (data != NULL)
    {
        data->pValid = data->pValidTail = data->pHead;
        data->pop_size = data->push_size;
    }    
}

int TBuffer_Push(TBufffer Object, const void *data, int size)
{
    int result = 0;
    int _ValidLength = Object->push_size - Object->pop_size;
    int _Size = Object->size - _ValidLength;    //实际可用 为0？
    if (data == NULL)
        result = -1;
    else if (size > 0 && _Size > 0) //保留原本不能丢弃
    {
        if (_Size < size)
            size = _Size;
        result = size;
        if (Object->pValidTail + size > Object->pTail)//写入之后超出缓冲区 分两段
        {
            int rest = Object->pTail - Object->pValidTail;
            memcpy(Object->pValidTail, data, rest);
            memcpy(Object->pHead, (char *)data + rest, size - rest);
            Object->pValidTail = Object->pHead + size - rest;
        }
        else
        {
            memcpy(Object->pValidTail, data, size);
            Object->pValidTail += size;
            if (Object->pValidTail == Object->pTail)
                Object->pValidTail = Object->pHead;
        }
        Object->push_size += result;
    }
    // printf(" push = %s\n", Object->pHead);  
    return result;
}

int TBuffer_Peek(TBufffer Object, void *data, int size)//返回实际尺寸
{
    int result = 0;
    if (data == NULL)
        result = -1;
    else if (size > 0 && Object->pop_size != Object->push_size)
    {
        int _ValidLength = Object->push_size - Object->pop_size;
        if (size > _ValidLength)//可能会push validLen变大 导致size变大越界
            size = _ValidLength;
        if (Object->pValid + size > Object->pTail)//需要分成两段copy
        {
            int rest = Object->pTail - Object->pValid;
            memcpy(data, Object->pValid, rest);
            memcpy((char*)data + rest, Object->pHead, size - rest);//第二段，绕到整个存储区的开头
        }
        else
        {
            memcpy(data, Object->pValid, size);
        }
        result = size;
    }
    return result;
}

int TBuffer_Pop(TBufffer Object, void *data, int size)                                                                        
{
    int result = 0;
    if (data == NULL)
        result = -1;
    else if (size > 0 && Object->pop_size != Object->push_size)
    {
        int _ValidLength = Object->push_size - Object->pop_size;
        if (size > _ValidLength)//可能会push validLen变大 导致size变大越界
            size = _ValidLength;
        if (Object->pValid + size > Object->pTail)//需要分成两段copy
        {
            int rest = Object->pTail - Object->pValid;
            memcpy(data, Object->pValid, rest);
            memcpy((char*)data + rest, Object->pHead, size - rest);//第二段，绕到整个存储区的开头
            Object->pValid = Object->pHead + size - rest;//更新已使用缓冲区的起始
        }
        else
        {
            memcpy(data, Object->pValid, size);
            Object->pValid = Object->pValid + size;
            if (Object->pValid == Object->pTail)
                Object->pValid = Object->pHead;
        }
        result = size;
        Object->pop_size += result;
    }
    // printf("pop = %s\n",(char*)data);   
    return result;
}
