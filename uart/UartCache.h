/**
 * File name: UartCache.h
 * Author: even
 * Email: 871454583@qq.com
 * Version: 1.0
 * Created on: 2017-10-11
 * Description:
 */

#ifndef UARTCACHE_H_
#define UARTCACHE_H_

#include "../common.h"
#include "../utils/SocketUtils.h"
#include "../socket/SocketMsg.h"
#include "IOnFindUartListener.h"

class UartCache {
public:

	UartCache(IOnFindUartListener *listener);
	virtual ~UartCache();
	bool MsgPreParse(int fd, uint8_t *buf, int len, void *args);	// 通信数据解析
//	bool UartRevArray(uint8_t iarray,uint8_t *buf,int len,void *args, Msg* msg);
//private:
	uint8_t mbuf[MSG_CACHE_SIZE];
	int mfront;
	int mrear;
	int mcurrent;
	int mlen;
	int mtag;
	SearchStrategy strategy;
	IOnFindUartListener *mFindUartListener;

	void *args;
	Msg msg;

	int __MsgCopyIn(uint8_t *buf, int len);	// 将数据添加入缓存
	int __MsgCopyOut(uint8_t *buf, int start_index, int len);	//将数据移出缓存
	int __MsgParse(int fd);		// 通信数据解析
};

inline UartCache::UartCache(IOnFindUartListener *listener):
		mfront(0),mrear(0), mcurrent(0), mlen(0), mtag(0), strategy(SEARCH_HEAD), mFindUartListener(
				listener), args(NULL)
{
	printf("UartCache object created!\n");
}

inline UartCache::~UartCache() {
	printf("UartCache object destroyed!\n");
}

inline int UartCache::__MsgCopyIn(uint8_t* buf, int len) {
	int left_len;	// 剩余长度
	int copy_len;
	int count;
	uint8_t* src = buf;

	if (this->mtag == 1 && this->mfront == this->mrear) {
#if DEBUG
		printf("uart cache is full! tag is %d \n" , this->mtag);
#endif
		this->mtag = 0;
		this->strategy = SEARCH_HEAD;
		return 0;
	}

	left_len = MSG_CACHE_SIZE - this->mlen;
	copy_len = len > left_len ? left_len : len;
	count = copy_len;

	if(count == 0) return 0;
	while (count--) {
		*(this->mbuf + this->mrear) = *src;	// 入队
		src++;
		this->mrear = (this->mrear + 1) % MSG_CACHE_SIZE;
		this->mlen++;//收到的指令的总长度
		this->mtag = 1;
	}

	return copy_len;
}

inline int UartCache::__MsgCopyOut(uint8_t *buf, int start_index, int len) {
	uint8_t *dest;
	int src;
	int count;
	if (this->mbuf == NULL || buf == NULL || len == 0) {
		return 0;
	}
	if (this->mfront == this->mrear && this->mtag == 0) {
#if DEBUG
		printf("uart  cache is empty!");
#endif
		return 0;
	}
	if (this->mrear > this->mfront) {	// 如果循环队列不为空
		if (start_index < this->mfront || start_index > this->mrear) {
#if DEBUG
			printf("invalid start index!");
#endif
			return 0;
		}
	} else if (start_index > this->mrear && start_index < this->mfront) {
#if DEBUG
		printf("invalid start index!");
#endif
		return 0;
	}

	src = start_index;
	dest = buf;
	count = len;
	while (count--) {
		*dest = *(this->mbuf + src);
		dest++;
		src = (src + 1) % MSG_CACHE_SIZE;
	}
	return len;
}

inline int UartCache::__MsgParse(int fd) {
	int currLen;
	int p, q;
	int i;
	bool find;
	int recvDataLen;
	//Msg* msg = pmsg;

	if (this->mfront == this->mrear && this->mtag == 0) { // Cache为空
		return 0;
	}

	if (this->mcurrent >= this->mfront) {
		currLen = this->mlen - (this->mcurrent - this->mfront);
	} else {
		currLen = this->mrear - this->mcurrent;
	}

#if DEBUG
	int ioffset;
	printf("strategy is : %x. data is \n",this->strategy);
	for(ioffset=0;ioffset<currLen;ioffset++)
	{
		printf("%x  ",this->mbuf[this->mcurrent + ioffset]);
	}
#endif
	switch (this->strategy) {
	case SEARCH_HEAD:
		if (currLen < MSG_HEAD_SIZE) {
//			this->mcurrent = (this->mcurrent + currLen) % MSG_CACHE_SIZE;//更新到最新位置
			this->mcurrent = this->mfront;
			return 0;
		}
		find = false;
		for (i = 0; i < currLen - 1; i++) {
			p = this->mcurrent;
			q = (this->mcurrent + 1) % MSG_CACHE_SIZE;
			if ((this->mbuf[p] == (MSG_RECV_HEAD >> 8)) && (this->mbuf[q] == (MSG_RECV_HEAD & 0xff))) {
				this->msg.isRecv = true;
				find = true;
				break; // exit for loop
			} else {
				// current pointer move to next
				this->mcurrent = q;
				// delete one item
				this->mfront = this->mcurrent;
				this->mlen--;
				this->mtag = 0;
			}
		}

		if (find == true) {
			//move 2 items towards next
			this->mcurrent = (this->mcurrent + 2) % MSG_CACHE_SIZE;
			//we found the head format, go on to find Type byte
			this->strategy = SEARCH_CMD;
		} else {
			//if there is no head format ,delete previous items
			printf("uart message without head: %x!\n", MSG_RECV_HEAD);
			//go on to find Head format
			this->mcurrent = (this->mcurrent + 1) % MSG_CACHE_SIZE;//更新到最新位置
			this->mfront = this->mcurrent;
			this->strategy = SEARCH_HEAD;
		}
		break;

	case SEARCH_CMD: //to find the type direction in cache
		if (currLen < MSG_CMD_SIZE) {
//			this->mcurrent = (this->mcurrent + currLen) % MSG_CACHE_SIZE;//更新到最新位置
			this->mcurrent = this->mfront;
			this->strategy = SEARCH_HEAD;
			return 0;
		}
		//get the value of type
		this->msg.cmd = this->mbuf[this->mcurrent];
		if (this->msg.isRecv)
		{
			recvDataLen = 0x0B;
			this->msg.dataLen = recvDataLen;
		}

		else
		{
			//if there is no head format ,delete previous items
			printf("socket message direction is error: %x!\n", this->msg.cmd);
			//go on to find Head format
			this->strategy = SEARCH_HEAD;
		}
		if (this->strategy == SEARCH_HEAD) {
			//delete the first item 'a5'
			//move back 1 items
			this->mcurrent =
					this->mcurrent >= 1 ? (this->mcurrent - 1) : (MSG_CACHE_SIZE - 1 + this->mcurrent);
			this->mfront = this->mcurrent;
			this->mlen -= 1;
			this->mtag = 0;
//			this->strategy = SEARCH_HEAD;
		} else {
			this->mcurrent = (this->mcurrent + 1) % MSG_CACHE_SIZE;
			this->strategy = SEARCH_END;
		}
		break;

//	case SEARCH_LEN: //to find the type len in cache
//		if (currLen < MSG_CMD_SIZE) {
////			this->mcurrent = (this->mcurrent + currLen) % MSG_CACHE_SIZE;//更新到最新位置
//			this->mcurrent = this->mfront;
//			this->strategy = SEARCH_HEAD;
//			return 0;
//		}
//		//get the value of type
//		recvDataLen = this->mbuf[this->mcurrent];
//		if(this->msg.dataLen != recvDataLen){
//			this->mcurrent = (this->mcurrent + currLen) % MSG_CACHE_SIZE;//更新到最新位置
//			this->mfront = this->mcurrent;
//			return 0;
//		}
//		if (recvDataLen < 0) {
//			//delete the first item 'a5'
//			//move back 1 items
//			this->mcurrent =
//					this->mcurrent >= 1 ? (this->mcurrent - 1) : (MSG_CACHE_SIZE - 1 + this->mcurrent);
//			this->mfront = this->mcurrent;
//			this->mlen -= 1;
//			this->mtag = 0;
//			this->strategy = SEARCH_HEAD;
//		} else {
//			this->msg.dataLen = recvDataLen;
//			this->mcurrent = (this->mcurrent + 1) % MSG_CACHE_SIZE;
//			this->strategy = SEARCH_END;
//		}
//		break;

	case SEARCH_END:
		if (currLen < (this->msg.dataLen + MSG_CRC_SIZE + MSG_END_SIZE))
		{
//			this->mcurrent = (this->mcurrent + currLen) % MSG_CACHE_SIZE;//更新回去
			this->mcurrent = this->mfront;
			this->strategy = SEARCH_HEAD;
			return 0;
		}
		// Because we have known the data bytes' length, so we move the very
		// distance of data_len to see if there is End format.
		p = (this->mcurrent + this->msg.dataLen + MSG_CRC_SIZE) % MSG_CACHE_SIZE;
		if (this->mbuf[p] == MSG_END) {		// 匹配到完整指令
			__MsgCopyOut(this->msg.data, this->mcurrent, this->msg.dataLen+MSG_CRC_SIZE);
			// Delete all previous items.
			this->mcurrent = (p + 1) % MSG_CACHE_SIZE;
			this->mfront = this->mcurrent;
			this->mlen -= (this->msg.dataLen + SOCKET_MSG_FORMAT_SIZE);
			this->mtag = 0;

			if (this->msg.CRCCheck() && mFindUartListener != NULL) {
				// this->msg.Update() 更新SocketMsg参数，buf、len等
				mFindUartListener->onFindUartInstruction(fd, this->msg.Update(), this->args);
			}
			//muartServer->DataSorting(fd, this->msg.Update(), this->args);
			// this->msg.Update() 更新SocketMsg参数，buf、len等
			//mFindMsgListener->onFindInstruction(fd, this->msg.Update(), this->args);
		} else {
			printf("socket message without end: %x! IS %x\n", MSG_END ,this->mbuf[p]);
			// Delete the first item 'a5'
			// Move back 2 items
			this->mcurrent =
					this->mcurrent >= 3 ? (this->mcurrent - 3) : (MSG_CACHE_SIZE - 3 + this->mcurrent);
			this->mfront = this->mcurrent;
			// Length sub 2
			this->mlen -= 1;
			this->mtag = 0;
		}
		this->strategy = SEARCH_HEAD;
		break;

	default:
		break;
	}
	// Parse new socket message.
	return __MsgParse(fd);
}

//来源指针，数据起始地址，数据长度，空
inline bool UartCache::MsgPreParse(int fd, uint8_t *buf, int len, void *args) {
	int n = 0;
	uint8_t *p = buf;
	// When reading buffer's length is greater than cache's left length,
	// We should copy many times.
	this->msg.f_dsource = fd;//配置来源信道
	this->args = args;
	while (1) {
		n = __MsgCopyIn(p, len);
		if (n == 0) {
			return false;		// Cache is full
		}
		// Parse and handle socket message from cache
		__MsgParse(fd);
		if (n == len) {
			return true; // Copy completed
		}
		// Move the pointer

		p = p + n;
		len = len - n;
	}

	return true;
}
#endif /* UartCache */
