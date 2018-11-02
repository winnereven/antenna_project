/**
 * File name: IOnDownCmdListener.h
 * Author: JoshuaWong
 * Email: 1034316376@qq.com
 * Version: 1.0
 * Created on: Jun 29, 2017 4:21:03 PM
 * Description: ����ָ���������SocketServer����������ָ��ʱ����
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
	//����ָ���
	virtual Msg* onGetDownCmd(Msg* down) = 0;
};

#endif /* SOCKET_IONGETDOWNCMDLISTENER_H_ */
