/**
 * File name: SocketMsg.h
 * Author: JoshuaWong
 * Email: 1034316376@qq.com
 * Version: 1.0
 * Created on: Jun 27, 2017 11:31:02 PM
 * Description: 
 */

#ifndef SOCKET_SOCKETMSG_H_
#define SOCKET_SOCKETMSG_H_

#include "../common.h"
#include "../utils/CRC16Utils.h"

class Msg {
public:
	int f_dsource;//配置来源信道
	uint8_t cmd;
	uint8_t data[SOCKET_MSG_DATA_SIZE];
	uint16_t crc;
	int iswait;
	int dataLen;
	bool isRecv;

	Msg();
	// cmd:操作码	buf:指令数据	len:数据长度	isRecv:是否为下行指令
	Msg(int ds,uint8_t cmd, uint8_t *data, int dataLen, bool isRecv);
	~Msg();
	Msg* Update();
	bool CRCCheck();

	int CopyToBuf(uint8_t *container, int start_index);

private:
	uint8_t buf[SOCKET_MSG_SIZE];
	int len;
};

inline Msg::Msg() :
		f_dsource(0),cmd(-1), crc(0xffff), dataLen(0), isRecv(true) {
	len = 0;
}

inline Msg::Msg(int ds,uint8_t cmd, uint8_t *data, int dataLen, bool isRecv) :
		crc(0) {
	len =0;
	this->f_dsource = ds;
	if (dataLen < 0) {
		return;
	}
	this->cmd = cmd;
	memcpy(this->data, data, dataLen);
	this->dataLen = dataLen;
	this->isRecv = isRecv;

	this->Update();
}

inline Msg::~Msg() {
}

inline Msg* Msg::Update() {
	// HEAD
	if (this->isRecv) {
		this->buf[0] = MSG_RECV_HEAD & 0xff;
		this->buf[1] = MSG_RECV_HEAD >> 8;
	} else {
		this->buf[0] = MSG_SEND_HEAD & 0xff;
		this->buf[1] = MSG_SEND_HEAD >> 8;
	}
	// CMD
	this->buf[2] = this->cmd;
	// DATA
	if (this->data != NULL && this->dataLen != 0)
		memcpy(this->buf + 3, this->data, this->dataLen);
	// CRC
	this->crc = CRC16Utils::CRC16(this->buf, this->dataLen + 3);
	this->buf[this->dataLen + 3] = this->crc >> 8;
	this->buf[this->dataLen + 4] = this->crc & 0xff;
	// END
	this->buf[this->dataLen + 5] = MSG_END;

	this->len = this->dataLen + SOCKET_MSG_FORMAT_SIZE;
	return this;
}

inline bool Msg::CRCCheck() {
	uint16_t crc = this->data[this->dataLen] * 256 + this->data[this->dataLen + 1];
	Update();
	return 1;
//	return this->crc == crc;
}

inline int Msg::CopyToBuf(uint8_t *container, int start_index) {
	memcpy(container + start_index, this->buf, this->len);
	return this->len;
}

#endif /* SOCKET_SOCKETMSG_H_ */
