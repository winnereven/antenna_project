/**
 * File name: IOnFindInstructionListener.h
 * Author: JoshuaWong
 * Email: 1034316376@qq.com
 * Version: 1.0
 * Created on: Jun 27, 2017 11:45:14 PM
 * Description: 消息循环队列消息解析监听，当解析出合法指令时触发
 */

#ifndef SOCKET_IONFINDMSGLISTENER_H_
#define SOCKET_IONFINDMSGLISTENER_H_

#include "SocketMsg.h"

class IOnFindMsgListener {
public:
	IOnFindMsgListener() {
	}
	virtual ~IOnFindMsgListener() {
	}
	virtual void onFindInstruction(int fd, Msg *msg, void *args) = 0;

};

#endif /* SOCKET_IONFINDMSGLISTENER_H_ */
