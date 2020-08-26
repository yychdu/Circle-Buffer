#pragma once
#include <stdio.h>
#include <string.h>
#include <array>
#include <malloc.h>
#include <stdlib.h>
//基础类型
typedef unsigned char  Byte;
typedef unsigned short Word;
typedef unsigned int   DWord;

/* TObject - 基类 */

class TObject{};

/* TRingBuffer - 环形缓冲区类 */

// 注：为了多线程存取安全，Push 和 Pop 分属两个线程时可以同时操作而不需要加锁，
//     但多线程 Push 时必须用锁控制，多线程 Pop/Peek/Lose 时必须用锁控制！

class TRingBuffer {
public:
	// 读写数据的方法，返回数据尺寸
	typedef int (TObject::* TDoRead)(void* AData, int ASize);
	typedef int (TObject::* TDoWrite)(const void* AData, int ASize);

	// 读写数据的 Push 和 Pop 方法类型
	// 注：因为 Push 和 Pop 存在重载方法，编译器无法指定哪个方法，所以明确定义方
	// 法类型，可以让编译器识别到具体哪个方法。
	typedef DWord(TRingBuffer::* TDoPop)(void* AData, DWord ASize);
	typedef DWord(TRingBuffer::* TDoPush)(const void* AData, DWord ASize);

public:
	// 构造函数
	// AMaxSize 缓冲区尺寸，AMaxSize >= 16
	TRingBuffer(int AMaxSize = 1024);
	~TRingBuffer();

	// 属性
	DWord		Size() const { return push_size - pop_size; }
	DWord		MaxSize() const { return max_size; }
	DWord		PopSize() const { return pop_size; }
	DWord		PushSize() const { return push_size; }

	//数据加入缓冲区
	DWord Push(TDoRead ADoRead, void* AObject, DWord ASize);
	DWord Push(const void* AData, DWord ASize);
	DWord Push(TRingBuffer& ABuffer, DWord ASize) {
		return Push((TDoRead)(TDoPop)&TRingBuffer::Pop, &ABuffer, ASize);
	}
	DWord Push(TRingBuffer& ABuffer) {
		return Push(ABuffer, 0xFFFFFFFF);
	}

	// 从缓冲区读取数据，返回读取的数据尺寸
	DWord Pop(TDoWrite ADoWrite, void* AObject, DWord ASize);
	DWord Pop(void* AData, DWord ASize);
	DWord Pop(TRingBuffer& ABuffer, DWord ASize) {
		return Pop((TDoWrite)(TDoPush)& TRingBuffer::Push, &ABuffer, ASize);
	}
	DWord Pop(TRingBuffer& ABuffer) {
		return Pop(ABuffer, 0xFFFFFFFF);
	}

	// 从缓冲区中预读数据，返回预读取的数据尺寸
	DWord Peek(TDoWrite ADoWrite, void* AObject, DWord ASize) const;
	DWord Peek(void* AData, DWord ASize) const;
	DWord Peek(TRingBuffer& ABuffer, DWord ASize) const {
		return Peek((TDoWrite)(TDoPush)& TRingBuffer::Push, &ABuffer, ASize);
	}
	DWord Peek(TRingBuffer& ABuffer) const {
		return Peek(ABuffer, 0xFFFFFFFF);
	}

	// 从缓冲区求其指定尺寸的数据，返回丢弃的数据尺寸
	DWord Lose(DWord ASize);
	DWord Lose() { return Lose(0xFFFFFFFF); }

protected:

private:
	DWord max_size;
	DWord pop_size;
	DWord push_size;
	char* buf;
	char* start_idx;
	char* end_idx;
	char* end;
};



/*Push(TDoRead ADoRead, void* AObject, DWord ASize)
{
	// ??? ... ...
}
class TDemo
{
public:
	int   Recv(void* ABuffer, int Size);

};
{
	TRingBuffer buffer;
	TDemo       demo;
	char        data[1024];

	// ??? ... ...
	// method1   
	memcpy(data, aaa, len);
	len = unpack808(data, len);
	int size1 = buffer.Push(data, len);
	<=> memcpy(buffer.item + pos, size);
	memcpy(buffer.item, len - size);

	// method2
	typedef int (*_TDoRead)(void* obj, void* data, int size);
	typedef union
	{
		_TDoRead             Func;
		TRingBuffer::TDoRead Method;
	} TUniDoRead;

	int size2 = buffer.Push((TRingBuffer::TDoRead) & TDemo::Recv, &demo, 1000);
	<=> int len1 = ((TObject*)AObject->*ADoRead)(buffer.item + pos, size);
	if (len1 == size)
	{
		len1 = ((TObject*)AObject->*ADoRead)(buffer.item, len - size);
		if (len1 > 0)
			len1 += size;
		else
			len1 = size;
	}
}/*

