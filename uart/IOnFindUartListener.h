/**
 * File name: IOnFindInstructionListener.h
 * Author: JoshuaWong
 * Email: 1034316376@qq.com
 * Version: 1.0
 * Created on: Jun 27, 2017 11:45:14 PM
 * Description: 消息循环队列消息解析监听，当解析出合法指令时触发
 */

#ifndef UART_IONFINDMSGLISTENER_H_
#define UART_IONFINDMSGLISTENER_H_

#include "../socket/SocketMsg.h"

class IOnFindUartListener {
public:
	IOnFindUartListener() {
	}
	virtual ~IOnFindUartListener() {
	}
	//串口接收指令处理函数
	virtual void onFindUartInstruction(int fd, Msg *msg, void *args) = 0;

};

#endif /* UART_IONFINDMSGLISTENER_H_ */
