/**
 * File name: main.cpp
 * Author: JoshuaWong
 * Email: 1034316376@qq.com
 * Version: 1.0
 * Created on: 2017年6月15日 下午5:06:18
 * Description: 
 */

/**
 * main.cpp
 * Author: JoshuaWong JoshuaWong
 * Email: 1034316376@qq.com
 * Created on: 2016年11月28日 上午10:59:15
 * Description:
 */
#include <pthread.h>

#include "common.h"
#include "CDigitronSPI.h"
#include "CKeyScan.h"
#include "EventDriver.h"
#include "CTehu485.h"
#include "CMotor.h"

CDigitronSPI* mDigitron;
CKeyScan* mKeyProcessor;
EventDriver* mEventDriver;
CMotor* mMotor;

/**
 * 数码管显示线程
 */
void *DigDisplayThread(void *arg) {
	mDigitron->Display();
	pthread_exit(NULL);
}

/**
 * 电机运行线程
 */
void *MotorRunThread(void *arg) {
	mMotor->Run();
	pthread_exit(NULL);
}

void onClick(uint8_t key) {
	printf("key click : %d\n", key);
	if (mEventDriver != NULL) {
		mEventDriver->KeyClick((int) key);
	}
}

class Test: public OnLongClickListener {
protected:
	~Test();
public:
	void OnLongClick(uint8_t key) {
		printf("OnLongClickListener:%d\n", key);
		char s[4];
		sprintf(s, "%4d", key);
		mDigitron->SetText(s);
	}
};

// 电机运行
void initMotor() {
	mMotor = new CMotor();
	pthread_t tdMotor;
	int ret = pthread_create(&tdMotor, NULL, MotorRunThread, NULL);
	if (ret != 0)
		handle_error_en(ret, "Motor thread create failed!");
}

// 数码管初始化
void initDigitron() {
	mDigitron = new CDigitronSPI();
	pthread_t tdDisplay;

	int ret = pthread_create(&tdDisplay, NULL, DigDisplayThread, NULL);
	if (ret != 0)
		handle_error_en(ret, "Display thread create failed!");

//	dig.StopDisplay();
//
//	ret = pthread_join(tdDisplay, NULL);
//	if (ret != 0)
//		handle_error_en(ret, "Display thread exit exception!");
}

/**
 * 温湿度　Temperature Humidity
 */
void thTest() {
	CTehu485 tehu;
	tehu.Send();
	tehu.Read();
}

/**
 * 按键测试
 */
void initKey() {
	mKeyProcessor = new CKeyScan();
	mKeyProcessor->SetOnClickListener(onClick);
//	key->SetOnLongClickListener(new Test());
	mKeyProcessor->StartScan();
}
void initEventDriver() {
	mEventDriver = new EventDriver(mDigitron);
}
int main() {
	initMotor();
	initDigitron();
	initEventDriver();
	initKey();
}

