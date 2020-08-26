#define _CRT_SECURE_NO_WARNINGS 1
#define _CRT_NONSTDC_NO_DEPRECATE 1
#include <stdio.h>
#include <string.h>
#include "stdlib.h"
#include "pthread.h"
#include "stdbool.h"
#include "signal.h"
#include "windows.h"
#pragma comment(lib,"pthreadVC2.lib")
#include "buffer.h"
//用push_count 压入
//检查pop_count-push_count ? 
//错误休眠
//错误打印信息
//数据流连续压入 不能断开
char str[20] = {'1','2','3','4','5','6','7','8','9','a','b','c','d','e','f','w','a','1','2','3'};

bool run_over = false;

void sighandler(int signum)
{
    run_over = true;
}

void *push(void *arg)
{
    int RetSize = 0;
    int Push_Ptr = 0;
    TBufffer tmp = (TBufffer)arg;
    if (tmp != NULL)
    {
        while (!run_over)
        {
            RetSize = TBuffer_Push(tmp, str + Push_Ptr, sizeof(str) - Push_Ptr);
            if (RetSize < sizeof(str) - Push_Ptr)
            {
                Push_Ptr += RetSize;
                Sleep(1);   
            }
            else
                Push_Ptr = 0;
        }
    }
    else
        printf("TBuffferData  ERR");
    return NULL;
}


void *pop(void *arg)
{
    char buf[10];
    int RetSize = 0;
    int Pop_Ptr = 0;
    TBufffer tmp = (TBufffer)arg;
    if (tmp != NULL)
    {
       while (!run_over)
       {
            RetSize = TBuffer_Pop(tmp, buf, sizeof(buf) - 1);//size?
            if (RetSize == 0)   //无有效数据
               Sleep(1);
            else        //检查
            {
                // char str[20] = {'1','2','3','4','5','6','7','8','9','a','b','c','d','e','f','w','a','1','2','3'};
                //1.开始比较 比较完之后 Pop_Ptr指针往后移动RetSize
                //2.需要回到模板头部时 即 RetSize > sizeof(str) - Pop_Ptr 
                //分成两部分比较 第一部分比较 (sizeof(str) - Pop_Ptr)个长度，Pop_Ptr清零
                //第二部分比较 RetSize - (sizeof(str) - Pop_Ptr)个长度
                if (RetSize > sizeof(str) - Pop_Ptr)
                {
                    int PreSize = sizeof(str) - Pop_Ptr;
                    if (memcmp(str + Pop_Ptr, buf, PreSize) != 0 || memcmp(str, buf + PreSize, RetSize - PreSize) != 0)//?
                    {
                        puts("ERR 1");
                        run_over = true;
                    }
                    Pop_Ptr = RetSize - PreSize;
                }
                else
                {
                    if (memcmp(str + Pop_Ptr, buf, RetSize) != 0)
                    {
                        puts("ERR 2");
                        run_over = true;
                    }
                    Pop_Ptr += RetSize;//到头处理
                    if (Pop_Ptr == sizeof(str))
                        Pop_Ptr = 0;
                }
            }
        }        
    }
    else
       printf("TBuffferData ERR");
    return NULL;
}

TBuffferData _data;

int main() 
{
     char buf[16];
      TBufffer tmp = TBuffer_Init(&_data, buf, sizeof(buf));
    if (tmp != NULL)
    {
        pthread_t th1, th2;
        signal(SIGINT, sighandler);
        pthread_create(&th1, NULL, push, tmp);
        pthread_create(&th2, NULL, pop, tmp);
        
        pthread_join(th1, NULL);
        pthread_join(th2, NULL);
    }
    else
        printf("Failed to initialize");
    return 0;
}     