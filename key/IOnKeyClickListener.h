/**
 * File name: IOnKeyClickListener.h
 * Author: JoshuaWong
 * Email: 1034316376@qq.com
 * Version: 1.0
 * Created on: Jun 29, 2017 11:24:33 AM
 * Description: 按键监听，当有按键按下或本远控切换时触发
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
	// 按键事件
	virtual void onKeyClick(int key) = 0;
	// 本远控改变事件
	virtual void onCtlModeChanged(bool isLocal) = 0;
};

#endif /* KEY_IONKEYCLICKLISTENER_H_ */
