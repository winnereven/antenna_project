/**
 * File name: CTehu485.h
 * Author: JoshuaWong
 * Email: 1034316376@qq.com
 * Version: 1.0
 * Created on: 2017年5月31日 上午11:07:17
 * Description: 
 */

#ifndef CTEHU485_H_
#define CTEHU485_H_

#include <fcntl.h>
#include <sys/ioctl.h>
#include "common.h"
#include "./sys/serial.h"
#include "./sys/config.h"

#define DEV_UART1 "/dev/ttySP1"
#define DEV_UART2 "/dev/ttySP4"
//#define DEV_485_EN "/dev/gpio-P3.26"
static uchar WorkingCondition[10];//工况参数存放位置
const uchar TH_Send_Command[] = { 0x80, 0x10, 0x10, 0x02, 0x00, 0xee, 0xee, 0xff };
//收到的数据 80 10 10 02 03  34  12  75 ee ee ff
//表示						23.4度  75湿度
const uchar BH_Send_Command[] = { 0x01, 0x04, 0x00, 0x00, 0x00, 0x09, 0x30, 0x0c };
//收到的数据  1 4 1 38 61 40 c0 80 0 0 0 0 0 0 0     9 23 0 0 0   0    9a 1b
//				     电压				A	B	C电流		               状态		校验
class CTehu485 {
public:
	CTehu485();
	~CTehu485();
	void get_WorkStation(bool on);
	uchar *back_WorkStation();
private:
	// 串口1设备标志
	int m_fdUart1;
	int m_fdUart2;
//	int m_fd485En;
	void SendWSD();
	void ReadWSD();
	void SendBHQ();
	void ReadBHQ();

//	void Set485En();
//	void Clear485En();
};

inline CTehu485::CTehu485() {
	printf("CTehu485 object created!\n");
	m_fdUart1 = open(DEV_UART1, O_RDWR | O_NOCTTY);
	if (m_fdUart1 < 0)
		handle_error_en(m_fdUart1, "open Uart1 device failed");
	int ret = set_port_attr(m_fdUart1, UART_BAUD_RATE_M, UART_DATA_BIT, UART_STOP_BIT, UART_PARITY,
			UART_VTIME, UART_VMIN);
	if (ret < 0) {
		printf("set uart1 attr failed \n");
		exit(-1);
	}
	m_fdUart2 = open(DEV_UART2, O_RDWR | O_NOCTTY);
	if (m_fdUart2 < 0)
		handle_error_en(m_fdUart2, "open Uart2 device failed");
	ret = set_port_attr(m_fdUart2, UART_BAUD_RATE_M, UART_DATA_BIT, UART_STOP_BIT, UART_PARITY,
			UART_VTIME, UART_VMIN);
	if (ret < 0) {
		printf("set uart4 attr failed \n");
		exit(-1);
	}
	//TODO  配置为非阻塞模式
	if(fcntl(m_fdUart1,F_SETFL,FNDELAY) < 0)
		printf("fcntl m_fdDEBUG failed\n");

	if(fcntl(m_fdUart2,F_SETFL,FNDELAY) < 0)
		printf("fcntl m_fdLIGHT1 failed\n");
	get_WorkStation(0);
	get_WorkStation(1);
}

inline CTehu485::~CTehu485() {
	int ret;
	ret = close(m_fdUart1);
	if (ret)
		handle_error_en(ret, DEV_UART1);
	ret = close(m_fdUart2);
	if (ret)
		handle_error_en(ret, DEV_UART2);
//	ret = close(m_fd485En);
//	if (ret)
//		handle_error_en(ret, DEV_485_EN);
	printf("CDigitronSPI object destroyed!\n");
}

inline void CTehu485::SendWSD() {
//	Clear485En();
	int len = write(m_fdUart1, TH_Send_Command, sizeof(TH_Send_Command));
	log("write data size : %d \n", len);
	if (len < 0) {
		log("write data to serial failed! \n");
	}
}

inline void CTehu485::ReadWSD() {
//	Clear485En();
	unsigned char ucBuf[20];
	int len = read(m_fdUart1, ucBuf, sizeof(ucBuf));
	if(DEBUG_even)
	{
		printf("get data: %d \n", len);
		for (int i = 0; i < len; i++) {
			printf(" %x", ucBuf[i]);
		}
		printf("\n");
	}
	if(ucBuf[0]==0x80&&ucBuf[3]==0x02)
	{
		WorkingCondition[0]=ucBuf[6];
		WorkingCondition[1]=ucBuf[5];
		WorkingCondition[2]=ucBuf[7];
		WorkingCondition[9]&=0xdf;
	}
	else{
		WorkingCondition[0]=0;
		WorkingCondition[1]=0;
		WorkingCondition[2]=0;
		WorkingCondition[9]|=0x20;
	}
}
inline void CTehu485::SendBHQ() {
	int len = write(m_fdUart2, BH_Send_Command, sizeof(BH_Send_Command));
	log("write data size : %d \n", len);
	if (len < 0) {
		log("write data to serial failed! \n");
	}
}
inline void CTehu485::ReadBHQ() {
	unsigned char ucBuf[40];
	int len = read(m_fdUart2, ucBuf, sizeof(ucBuf));
	if(DEBUG_even){
		printf("get data: %d \n", len);
		for (int i = 0; i < len; i++) {
			printf(" %x", ucBuf[i]);
		}
		printf("\n");
	}
	if(ucBuf[0]==0x01&&ucBuf[1]==0x04)
	{
		WorkingCondition[3]=ucBuf[10];
		WorkingCondition[4]=ucBuf[12];
		WorkingCondition[5]=ucBuf[14];
		if(ucBuf[3]*256+ucBuf[4]<4000)
		{
			WorkingCondition[6]=ucBuf[3];
			WorkingCondition[7]=ucBuf[4];
		}
		WorkingCondition[8]=ucBuf[20];
		WorkingCondition[9]&=0xBF;
	}
	else{
//		WorkingCondition[3]=0;
//		WorkingCondition[4]=0;
//		WorkingCondition[5]=0;
//		WorkingCondition[6]=0;
//		WorkingCondition[7]=0;
//		WorkingCondition[8]=0;
		WorkingCondition[9]|=0x40;
	}
}
inline void CTehu485::get_WorkStation(bool on)
{
	if(on){
		SendWSD();
		usleep(700000);
		ReadWSD();
	}else{
		SendBHQ();
		usleep(700000);
		ReadBHQ();
	}
//	return WorkingCondition;
}
inline uchar *CTehu485::back_WorkStation()
{

	return WorkingCondition;
}
#endif /* CTEHU485_H_ */
