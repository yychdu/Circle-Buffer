#include "RingBuffer.h"
#include <malloc.h>
#include <stdlib.h>

TRingBuffer::TRingBuffer(int AMaxSize)
	:pop_size(0), push_size(0){
	//检查AMaxSize
	if (AMaxSize <= 0) 
		max_size = 0;
	else if (AMaxSize > 0x7fffffff) 
		max_size = 0;
	else 
		max_size = AMaxSize;

	//分配内存
	buf = (char*)malloc(max_size);
	if (buf == NULL)
		max_size = 0;
	start_idx = buf;
	end_idx = buf;
	end = buf + max_size;
}

TRingBuffer::~TRingBuffer()
{
	free(buf);
}

DWord TRingBuffer::Push(TDoRead ADoRead, void* AObject, DWord ASize)
{

	DWord size = max_size - Size();
	DWord rest = end - end_idx;
	int ret;
	if (size != 0) {
		if (size < ASize)
			ASize = size;
		if (rest >= ASize) {
			ret = ((TObject*)AObject->*ADoRead)(end_idx, ASize);
			if (ret < 0)
				ret = 0;
			end_idx += ret;
		}
		else {
			ret = ((TObject*)AObject->*ADoRead)(end_idx, rest);
			if (ret == rest)
			{
				ret = ((TObject*)AObject->*ADoRead)(buf, ASize - rest);
				if (ret > 0) {
					end_idx = buf + ret;
					ret += rest;
				}
				else
					ret = rest;
			}
			else if (ret < 0)
				ret = 0;
			end_idx += ret;
		}
		if (end_idx == end)
			end_idx = buf;
		push_size += ret;
		return ret;
	}
	else 
		return 0;
}

DWord TRingBuffer::Push(const void* AData, DWord ASize)
{
	DWord size = max_size - Size();
	DWord rest = end - end_idx;
	if (size != 0) {
		if (size < ASize)
			ASize = size;
		if (rest >= ASize) {
			memcpy(end_idx, AData, ASize);
			end_idx += ASize;
		}
		else {
			memcpy(end_idx, AData, rest);
			memcpy(buf, (char*)AData + rest, ASize - rest);
			end_idx = buf + (ASize - rest);
		}
		push_size += ASize;
		if (end_idx == end)
			end_idx = buf;
		return ASize;
	}
	else 
		return 0;
}

DWord TRingBuffer::Pop(TDoWrite ADoWrite, void* AObject, DWord ASize)
{
	DWord size = Size();
	DWord rest = end - start_idx;
	int ret;
	if (size != 0) {
		if (size < ASize)
			ASize = size;
		if (rest >= ASize) {
			ret = ((TObject*)AObject->*ADoWrite)(start_idx, ASize);
			if (ret < 0)
				ret = 0;
			start_idx += ret;
		}
		else {
			ret = ((TObject*)AObject->*ADoWrite)(start_idx, rest);
			if (ret == rest)
			{
				ret = ((TObject*)AObject->*ADoWrite)(buf, ASize - rest);
				if (ret > 0) {
					start_idx = buf + ret;
					ret += rest;
				}
				else
					ret = rest;
			}
			else if (ret < 0)
				ret = 0;
			start_idx += ret;
		}
		if (start_idx == end)
			start_idx = buf;
		pop_size += ret;
		return ret;
	}
	else
		return 0;
}

DWord TRingBuffer::Pop(void* AData, DWord ASize)
{
	DWord size = Size();
	DWord rest = end - start_idx;
	if (size != 0) {
		if (size < ASize)
			ASize = size;
		if (rest >= ASize) {
			memcpy(AData, start_idx, ASize);
			start_idx += ASize;
		}
		else {
			memcpy(AData, start_idx, rest);
			memcpy((char*)AData + rest, buf, ASize - rest);
			start_idx = buf + (ASize - rest);
		}
		pop_size += ASize;
		if (start_idx == end)
			start_idx = buf;
		return ASize;
	}
	else
		return 0;
}

DWord TRingBuffer::Peek(TDoWrite ADoWrite, void* AObject, DWord ASize) const
{
	DWord size = Size();
	DWord rest = end - start_idx;
	int ret;
	if (size != 0) {
		if (size < ASize)
			ASize = size;
		if (rest >= ASize)
			ret = ((TObject*)AObject->*ADoWrite)(start_idx, ASize);
		else {
			ret = ((TObject*)AObject->*ADoWrite)(start_idx, rest);
			if (ret == rest)
			{
				ret = ((TObject*)AObject->*ADoWrite)(buf, ASize - rest);
				if (ret > 0)
					ret += rest;
				else
					ret = rest;
			}
		}
		if (ret > 0)
			return ret;
		else
			return 0;
	}
	else
		return 0;
}

DWord TRingBuffer::Peek(void* AData, DWord ASize) const
{
	DWord size = Size();
	DWord rest = end - end_idx;
	if (size < max_size) {
		if (max_size - size < ASize)
			ASize = max_size - size;
		if (rest >= ASize) {
			memcpy(end_idx, AData, ASize);
		}
		else {
			memcpy(end_idx, AData, rest);
			memcpy(buf, (char*)AData + rest, ASize - rest);
		}
		return ASize;
	}
	else
		return 0;
}

DWord TRingBuffer::Lose(DWord ASize)
{
	DWord size = Size();
	DWord rest = end - start_idx;
	if (size != 0) {
		if (size < ASize)
			ASize = size;
		if (rest >= ASize) {
			start_idx += ASize;
		}
		else {
			start_idx = buf + (ASize - rest);
		}
		pop_size += ASize;
		if (start_idx == end)
			start_idx = buf;
		return ASize;
	}
	else
		return 0;
}
