#ifndef __BUFFER
#define __BUFFER


typedef struct
{
    unsigned char  *pHead;//环形存储区的首地址
    unsigned char  *pTail;//环形存储区的结尾地址   
    unsigned char  *pValid;//已使用的缓冲区的首地址
    unsigned char  *pValidTail;//已使用的缓冲区的尾地址
    unsigned int pop_size;
    unsigned int push_size;
    unsigned int   size;
} TBuffferData;

typedef TBuffferData* TBufffer;
TBufffer TBuffer_Init(TBuffferData *data, void *buffer, int buff_size);
void TBuffer_Fini(TBuffferData *data);
int TBuffer_Push(TBufffer Object, const void *data, int size);
int TBuffer_Peek(TBufffer Object, void *data, int size); //返回实际尺寸
int TBuffer_Pop(TBufffer Object, void *data, int size);
#endif 
