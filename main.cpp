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
 * Author: even
 * Email: 871454583@qq.com
 * Created on: 2016年11月28日 上午10:59:15
 * Description:
 */
#include <pthread.h>

#include "common.h"
#include "CDigitronSPI.h"
#include "key/CKeyScan.h"
#include "EventDriver.h"
#include "CMotor.h"
#include "socket/SocketServer.h"
//#include "uart/UartServer.h"

CDigitronSPI* mDigitron;
CKeyScan* mKeyScan;
EventDriver* mEventDriver;
CMotor* mMotor;
SocketServer* mSocketServer;
//UartServer* mUartServer;//串口服务类

// 数码管显示线程
void *DigDisplayThread(void *arg) {
	mDigitron->Display();
	pthread_exit(NULL);
}

// 电机运行线程
void *MotorRunThread(void *arg) {
	mMotor->Run();
	pthread_exit(NULL);
}

// Socket Server线程
void *SocketServerThread(void *arg) {
	mSocketServer->Start();
	pthread_exit(NULL);
}
// Uart Server线程
//void *UartServerThread(void *arg) {
//	mUartServer->Start();
//	pthread_exit(NULL);
//}

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
//void thTest() {
//	CTehu485 tehu;
//	tehu.SendBHQ();
////	sleep(1);
//	tehu.ReadBHQ();
//}

/**
 * 按键测试
 */
void initKey() {
	mKeyScan = new CKeyScan();
}

void initSocketServer() {
	mSocketServer = new SocketServer();
	pthread_t tdSocket;

	int ret = pthread_create(&tdSocket, NULL, SocketServerThread, NULL);
	if (ret != 0)
		handle_error_en(ret, "Socket server thread create failed!");
}
//void initUartDriver()
//{
//	mUartServer = new UartServer();
//	pthread_t tdUart;
//
//	int ret = pthread_create(&tdUart, NULL, UartServerThread, NULL);
//	if (ret != 0)
//		handle_error_en(ret, "Uart server thread create failed!");
//}
void initEventDriver() {
//	mEventDriver = new EventDriver(mKeyScan, mDigitron, mSocketServer, mMotor,mUartServer);
	mEventDriver = new EventDriver(mKeyScan, mDigitron, mSocketServer, mMotor);

}

//void crctest() {
//	uint8_t buf[14] = { 0x24, 0x24, 0x2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
//	uint16_t crc = CRC16Utils::CRC16(buf, 14);
//	printf("%x %x\n", crc >> 8, crc & 0xff);
//}

int main(int argc, char* argv[]) {
	// 版本查询
	if(argc == 2){
		if(strcmp(argv[1], "--version") == 0){
			printf("version:%s\n", VERSION);
			exit(EXIT_SUCCESS);
		}
		else if(strcmp(argv[1], "--debug") == 0){
			DEBUG_even = 1;
		}
		else
		{
			printf("command error,  --version or --debug\n");
			exit(EXIT_SUCCESS);
		}
	}
	system("echo 1 > /sys/class/leds/beep/brightness");
	system("echo none > /sys/class/leds/led-run/trigger");
	initKey();
	initMotor();
	initDigitron();
	initSocketServer();
	initEventDriver();
	system("echo 0 > /sys/class/leds/beep/brightness");

	mKeyScan->StartScan();
}

