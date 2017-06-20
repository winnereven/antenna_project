/**
 * File name: main.cpp
 * Author: JoshuaWong
 * Email: 1034316376@qq.com
 * Version: 1.0
 * Created on: 2017��6��15�� ����5:06:18
 * Description: 
 */

/**
 * main.cpp
 * Author: JoshuaWong JoshuaWong
 * Email: 1034316376@qq.com
 * Created on: 2016��11��28�� ����10:59:15
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
 * �������ʾ�߳�
 */
void *DigDisplayThread(void *arg) {
	mDigitron->Display();
	pthread_exit(NULL);
}

/**
 * ��������߳�
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

// �������
void initMotor() {
	mMotor = new CMotor();
	pthread_t tdMotor;
	int ret = pthread_create(&tdMotor, NULL, MotorRunThread, NULL);
	if (ret != 0)
		handle_error_en(ret, "Motor thread create failed!");
}

// ����ܳ�ʼ��
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
 * ��ʪ�ȡ�Temperature Humidity
 */
void thTest() {
	CTehu485 tehu;
	tehu.Send();
	tehu.Read();
}

/**
 * ��������
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

