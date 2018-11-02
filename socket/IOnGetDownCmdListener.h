/**
 * File name: IOnDownCmdListener.h
 * Author: JoshuaWong
 * Email: 1034316376@qq.com
 * Version: 1.0
 * Created on: Jun 29, 2017 4:21:03 PM
 * Description: 下行指令监听，当SocketServer解析出下行指令时触发
 */

#ifndef SOCKET_IONGETDOWNCMDLISTENER_H_
#define SOCKET_IONGETDOWNCMDLISTENER_H_

#include "SocketMsg.h"

class IOnGetDownCmdListener {
public:
	IOnGetDownCmdListener() {
	}
	virtual ~IOnGetDownCmdListener() {
	}
	//接收指令处理
	virtual Msg* onGetDownCmd(Msg* down) = 0;
};

#endif /* SOCKET_IONGETDOWNCMDLISTENER_H_ */
