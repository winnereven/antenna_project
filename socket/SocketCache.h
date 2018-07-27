/**
 * File name: SocketCache.h
 * Author: JoshuaWong
 * Email: 1034316376@qq.com
 * Version: 1.0
 * Created on: Jun 27, 2017 11:35:33 PM
 * Description: 
 */

#ifndef SOCKET_SOCKETCACHE_H_
#define SOCKET_SOCKETCACHE_H_

#include "../common.h"
#include "../utils/SocketUtils.h"
#include "IOnFindMsgListener.h"
#include "SocketMsg.h"

class DataCache {
public:
	uint8_t buf[MSG_CACHE_SIZE];
	int front;
	int rear;
	int current;
	int len;
	int tag;
	SearchStrategy strategy;
	IOnFindMsgListener *mFindMsgListener;
	void *args;
	Msg msg;

	DataCache(IOnFindMsgListener *listener);
	~DataCache();
	int __MsgCopyIn(uint8_t *buf, int len);	// 将数据添加入缓存
	int __MsgCopyOut(uint8_t *buf, int start_index, int len);	//将数据移出缓存
	void __MsgParse(int fd);		// 通信数据解析
	bool MsgPreParse(int fd, uint8_t *buf, int len, void *args);
};

inline DataCache::DataCache(IOnFindMsgListener *listener) :
		front(0), rear(0), current(0), len(0), tag(0), strategy(SEARCH_HEAD), mFindMsgListener(
				listener), args(NULL) {
	printf("SocketCache object created!\n");
}

inline DataCache::~DataCache() {
	printf("SocketCache object destroyed!\n");
}

inline int DataCache::__MsgCopyIn(uint8_t *buf, int len) {
	int left_len;	// 剩余长度
	int copy_len;
	int count;
	uint8_t *src = buf;

	if (this->tag == 1 && this->front == this->rear) {
		log("socket cache is full!", " ");
		return 0;
	}

	left_len = MSG_CACHE_SIZE - this->len;
	copy_len = len > left_len ? left_len : len;
	count = copy_len;

	while (count--) {
		*(this->buf + this->rear) = *src;	// 入队
		src++;
		this->rear = (this->rear + 1) % MSG_CACHE_SIZE;
		this->len++;
		this->tag = 1;
	}

	return copy_len;
}

inline int DataCache::__MsgCopyOut(uint8_t *buf, int start_index, int len) {
	uint8_t *dest;
	int src;
	int count;
	if (this->buf == NULL || buf == NULL || len == 0) {
		return 0;
	}
	if (this->front == this->rear && this->tag == 0) {
		log("socket cache is empty!", " ");
		return 0;
	}
	if (this->rear > this->front) {	// 如果循环队列不为空
		if (start_index < this->front || start_index > this->rear) {
			log("invalid start index!", " ");
			return 0;
		}
	} else if (start_index > this->rear && start_index < this->front) {
		log("invalid start index!", " ");
		return 0;
	}

	src = start_index;
	dest = buf;
	count = len;
	while (count--) {
		*dest = *(this->buf + src);
		dest++;
		src = (src + 1) % MSG_CACHE_SIZE;
	}
	return len;
}

inline void DataCache::__MsgParse(int fd) {
	int currLen;
	int p, q;
	int i;
	bool find;
	int recvDataLen;

	if (this->front == this->rear && this->tag == 0) { // Cache为空
		return;
	}

	if (this->current >= this->front) {
		currLen = this->len - (this->current - this->front);
	} else {
		currLen = this->rear - this->current;
	}

	switch (this->strategy) {
	case SEARCH_HEAD:
		if (currLen < MSG_HEAD_SIZE) {
			return;
		}
		find = false;
		for (i = 0; i < currLen - 1; i++) {
			p = this->current;
			q = (this->current + 1) % MSG_CACHE_SIZE;
			if ((this->buf[p] == (MSG_RECV_HEAD >> 8))
					&& (this->buf[q] == (MSG_RECV_HEAD & 0xff))) {
				this->msg.isRecv = true;
				find = true;
				break; // exit for loop
			} else if ((this->buf[p] == (MSG_SEND_HEAD >> 8))
					&& (this->buf[q] == (MSG_SEND_HEAD & 0xff))) {
				this->msg.isRecv = false;
				find = true;
				break; // exit for loop
			} else {
				// current pointer move to next
				this->current = q;
				// delete one item
				this->front = this->current;
				this->len--;
				this->tag = 0;
			}
		}

		if (find == true) {
			//move 2 items towards next
			this->current = (this->current + 2) % MSG_CACHE_SIZE;
			//we found the head format, go on to find Type byte
			this->strategy = SEARCH_CMD;
		} else {
			//if there is no head format ,delete previous items
			printf("socket message without head: %x!\n", MSG_RECV_HEAD);
			//go on to find Head format
			this->strategy = SEARCH_HEAD;
		}
		break;

	case SEARCH_CMD: //to find the type byte in cache
		if (currLen < MSG_CMD_SIZE) {
			return;
		}
		//get the value of type
		this->msg.cmd = this->buf[this->current];
		if (this->msg.isRecv)
			recvDataLen = SocketUtils::GetDownstreamDataLen(this->msg.cmd);
		else
			recvDataLen = SocketUtils::GetUpstreamDataLen(this->msg.cmd);
		if (recvDataLen < 0) {
			//delete the first item 'a5'
			//move back 1 items
			this->current =
					this->current >= 1 ? (this->current - 1) : (MSG_CACHE_SIZE - 1 + this->current);
			this->front = this->current;
			this->len -= 1;
			this->tag = 0;
			this->strategy = SEARCH_HEAD;
		} else {
			this->msg.dataLen = recvDataLen;
			this->current = (this->current + 1) % MSG_CACHE_SIZE;
			this->strategy = SEARCH_END;
		}
		break;

	case SEARCH_END:
		if (currLen < (this->msg.dataLen + MSG_CRC_SIZE + MSG_END_SIZE))
			return;
		// Because we have known the data bytes' length, so we move the very
		// distance of data_len to see if there is End format.
		p = (this->current + this->msg.dataLen + MSG_CRC_SIZE) % MSG_CACHE_SIZE;
		if (this->buf[p] == MSG_END) {		// 匹配到完整指令
			__MsgCopyOut(this->msg.data, this->current, this->msg.dataLen + MSG_CRC_SIZE);
			// Delete all previous items.
			this->current = (p + 1) % MSG_CACHE_SIZE;
			this->front = this->current;
			this->len -= (this->msg.dataLen + SOCKET_MSG_FORMAT_SIZE);
			this->tag = 0;

			if (this->msg.CRCCheck() && mFindMsgListener != NULL) {
				// this->msg.Update() 更新SocketMsg参数，buf、len等
				mFindMsgListener->onFindInstruction(fd, this->msg.Update(), this->args);
			}
		} else {
			printf("socket message without end: %x!\n", MSG_END);
			// Delete the first item 'a5'
			// Move back 2 items
			this->current =
					this->current >= 2 ? (this->current - 2) : (MSG_CACHE_SIZE - 2 + this->current);
			this->front = this->current;
			// Length sub 2
			this->len -= 1;
			this->tag = 0;
		}
		this->strategy = SEARCH_HEAD;
		break;

	default:
		break;
	}

	// Parse new socket message.
	__MsgParse(fd);
}

inline bool DataCache::MsgPreParse(int fd, uint8_t *buf, int len, void *args) {
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

#endif /* SOCKET_SOCKETCACHE_H_ */
