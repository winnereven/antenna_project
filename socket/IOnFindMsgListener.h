/**
 * File name: IOnFindInstructionListener.h
 * Author: JoshuaWong
 * Email: 1034316376@qq.com
 * Version: 1.0
 * Created on: Jun 27, 2017 11:45:14 PM
 * Description: ��Ϣѭ��������Ϣ�������������������Ϸ�ָ��ʱ����
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
