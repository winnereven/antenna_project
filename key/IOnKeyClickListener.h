/**
 * File name: IOnKeyClickListener.h
 * Author: JoshuaWong
 * Email: 1034316376@qq.com
 * Version: 1.0
 * Created on: Jun 29, 2017 11:24:33 AM
 * Description: �������������а������»�Զ���л�ʱ����
 */

#ifndef KEY_IONKEYCLICKLISTENER_H_
#define KEY_IONKEYCLICKLISTENER_H_

#include "../common.h"

class IOnKeyClickListener {
public:
	IOnKeyClickListener() {

	}
	virtual ~IOnKeyClickListener() {
	}
	// �����¼�
	virtual void onKeyClick(int key) = 0;
	// ��Զ�ظı��¼�
	virtual void onCtlModeChanged(bool isLocal) = 0;
};

#endif /* KEY_IONKEYCLICKLISTENER_H_ */
