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

#define DEV_UART1 "/dev/ttySP1"
#define DEV_485_EN "/dev/gpio-URX4"

const uchar TH_Send_Command[] = { 0x80, 0x10, 0x10, 0x02, 0x00, 0x21, 0xbb, 0xff };

class CTehu485 {
public:
	CTehu485();
	~CTehu485();
	void Send();
	void Read();

private:
	// 串口1设备标志
	int m_fdUart1;
	int m_fd485En;

	void Set485En();
	void Clear485En();
};

inline CTehu485::CTehu485() {
	printf("CTehu485 object created!\n");
	m_fdUart1 = open(DEV_UART1, O_RDWR | O_NOCTTY);
	if (m_fdUart1 < 0)
		handle_error_en(m_fdUart1, "open Uart1 device failed");
	m_fd485En = open(DEV_485_EN, O_RDWR);
	if (m_fd485En < 0)
		handle_error_en(m_fd485En, "open 485EN device failed");
}

inline CTehu485::~CTehu485() {
	int ret;
	ret = close(m_fdUart1);
	if (ret)
		handle_error_en(ret, DEV_UART1);
	ret = close(m_fd485En);
	if (ret)
		handle_error_en(ret, DEV_485_EN);
	printf("CDigitronSPI object destroyed!\n");
}

inline void CTehu485::Send() {
	Clear485En();
	int len = write(m_fdUart1, TH_Send_Command, sizeof(TH_Send_Command));
	printf("write data size : %d \n", len);
	if (len < 0) {
		printf("write data to serial failed! \n");
	}
}

inline void CTehu485::Read() {
	Clear485En();
	unsigned char ucBuf[20];
	int len = read(m_fdUart1, ucBuf, sizeof(ucBuf));
	printf("get data: %d \n", len);
	for (int i = 0; i < len; i++) {
		printf(" %x", ucBuf[i]);
	}
	printf("\n");
}

inline void CTehu485::Set485En() {
	int ret = ioctl(m_fd485En, SET_GPIO_HIGHT);
	if (ret < 0)
		handle_err_log("%s %s", DEV_485_EN, "ioctl");
}

inline void CTehu485::Clear485En() {
	int ret = ioctl(m_fd485En, SET_GPIO_LOW);
	if (ret < 0)
		handle_err_log("%s %s", DEV_485_EN, "ioctl");
}

#endif /* CTEHU485_H_ */
