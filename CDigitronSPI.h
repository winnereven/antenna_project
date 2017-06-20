/**
 * File name: CDigitronSPI.cpp
 * Author: JoshuaWong
 * Email: 1034316376@qq.com
 * Version: 1.0
 * Created on: 2016��11��29������5:12:55
 * Description: 
 */
#ifndef CDIGITRONSPI_H_
#define CDIGITRONSPI_H_

#include <fcntl.h>
#include <sys/ioctl.h>

#include "common.h"
#include "Linux/spidev.h"
#include "Linux/gpio.h"
#include "utils/DigUtils.h"

#define DEV_SPI "/dev/spidev1.0"
#define DEV_REFRESH "/dev/gpio-UTX4-refresh"

const uint32_t SPI_MODE = SPI_MODE_0;
const uint32_t SPI_BITS = 8;
const uint32_t SPI_SPEED = 100000;
const uint16_t SPI_DELAY = 0;
const int DIS_DELAY_US = 3000;
const long BLINK_INTERVAL_US = 400 * 1000;

class CDigitronSPI {
public:
	CDigitronSPI();
	~CDigitronSPI();

	/**
	 * ��ʾ4λ����
	 * @param num:����ֵ
	 * @return
	 */
	void Display();
	/**
	 * ֹͣ��ʾ
	 * @return
	 */
	void StopDisplay();
	/**
	 * �����ʾ״̬
	 * @return true:displaying false:not
	 */
	bool GetDisplayStatus();
	/**
	 * ��ʾ��λ�����
	 * @param index:��ʾ�ַ��±�	pos:�����������λ��
	 * @param [out]
	 * @return
	 */
	void DisplayBit(uint8_t code, int pos, bool blank);
	/**
	 * ���ý���ʾ����
	 * @param num:��ʾ����
	 * @return
	 */
	void SetText(string str);
	/**
	 * ��������ܵ�ǰ���(Cursor)λ��
	 */
	void SetCursorPos(int pos);
	void CursorLeftShift();
	void CursorRightShift();
	/**
	 * ��������ܵ�ǰ���(Cursor)ֵ
	 */
	void CursorDataIncrease();
	void CursorDataDecrease();
	/**
	 * �Ƿ���ʾС����
	 */
	void SetPointEnable(bool b);
	/**
	 * ��ȡ���ýǶ�ֵ
	 */
	void GetDegree(int* degree);

	/**
	 * �����ʾ
	 * @param ��
	 * @return
	 */
	void Clear();

private:
	// SPI�豸��־
	int m_fdSPI;
	// GPIO������λ�Ĵ��������ʾ�豸��־
	int m_fdRefresh;
	// ��λ��ʾ�ַ�����
	int m_nDisCharCode[4];
	// ѭ����ʾ��־λ
	bool m_bFlagDisplay;
	// ��ǰ�����ѡ����˸λ��
	int m_nCursorPos;
	// �Ƿ���ʾС����
	bool m_bPoint;

	struct spi_ioc_transfer stSpiSetting[1];

	/**
	 * SPI���߳�ʼ������
	 * @param
	 * @return
	 */
	void __InitSPI();
};

inline CDigitronSPI::CDigitronSPI() :
		m_nCursorPos(-1), m_bPoint(false) {
	printf("CDigitronSPI object created!\n");
	m_fdSPI = open(DEV_SPI, O_RDWR);
	if (m_fdSPI < 0)
		handle_error_en(m_fdSPI, "open SPI device failed");
	m_fdRefresh = open(DEV_REFRESH, O_RDWR);
	if (m_fdRefresh < 0)
		handle_error_en(m_fdRefresh, "open Refresh device failed");
	Clear();
	m_bFlagDisplay = false;

	stSpiSetting[0].rx_buf = 0;
	stSpiSetting[0].len = 2;
//	stSpiSetting[0].delay_usecs = SPI_DELAY;
//	stSpiSetting[0].speed_hz = SPI_SPEED;
//	stSpiSetting[0].bits_per_word = SPI_BITS;

	__InitSPI();
}

inline CDigitronSPI::~CDigitronSPI() {
	int ret;
	ret = close(m_fdSPI);
	if (ret)
		handle_error_en(ret, DEV_SPI);
	ret = close(m_fdRefresh);
	if (ret)
		handle_error_en(ret, DEV_REFRESH);
	printf("CDigitronSPI object destroyed!\n");
}

inline void CDigitronSPI::SetText(string str) {
	Clear();
	const char* chars = str.c_str();
	int len = sizeof(chars) / sizeof(char);
	len = (len < 4 ? len : 4);
	for (int i = 0; i < len; i++) {
		m_nDisCharCode[i] = DigUtils::GetCodeOfChar(chars[len - i - 1]);
	}
}

inline void CDigitronSPI::Clear() {
	m_nDisCharCode[0] = DigUtils::GetBlankCode();
	m_nDisCharCode[1] = DigUtils::GetBlankCode();
	m_nDisCharCode[2] = DigUtils::GetBlankCode();
	m_nDisCharCode[3] = DigUtils::GetBlankCode();
}

inline void CDigitronSPI::StopDisplay() {
	m_bFlagDisplay = false;
}

inline bool CDigitronSPI::GetDisplayStatus() {
	return m_bFlagDisplay;
}

inline void CDigitronSPI::__InitSPI() {
	int ret;

	ret = ioctl(m_fdSPI, SPI_IOC_WR_MODE, &SPI_MODE);
	if (ret == 1)
		handle_error("cannot set SPI write mode!");
	ret = ioctl(m_fdSPI, SPI_IOC_RD_MODE, &SPI_MODE);
	if (ret == 1)
		handle_error("cannot set SPI read mode!");
	ret = ioctl(m_fdSPI, SPI_IOC_WR_BITS_PER_WORD, &SPI_BITS);
	if (ret != 0)
		handle_error("cannot set SPI write bits per word!");
	ret = ioctl(m_fdSPI, SPI_IOC_WR_MAX_SPEED_HZ, &SPI_SPEED);
	if (ret != 0)
		handle_error("cannot set SPI write max speed!");
	printf("spi mode: %d\n", SPI_MODE);
	printf("bits per word: %d\n", SPI_BITS);
	printf("max speed: %d Hz (%d KHz)\n", SPI_SPEED, SPI_SPEED / 1000);
}

inline void CDigitronSPI::Display() {
	int i = 0;
	m_bFlagDisplay = true;
	printf("digitron displaying...\n");

	long time = 0;
	bool blank = false;

	//ѭ����ʾ
	while (m_bFlagDisplay) {
		if (time > BLINK_INTERVAL_US) {
			blank = !blank;
			time = 0;
		}
		DisplayBit(m_nDisCharCode[i], i, blank);
		i++;
		i %= 4;
		usleep(DIS_DELAY_US);
		time += DIS_DELAY_US;
	}

//����
	i = 3;
	Clear();
	while (i--)
		DisplayBit(m_nDisCharCode[i], i, false);

	printf("digitron stop display...\n");
	m_bFlagDisplay = false;
}

inline void CDigitronSPI::DisplayBit(uint8_t code, int pos, bool blank) {
	int ret;
	if (pos < 0 || pos > 3)
		handle_error("Dig position is out of range!");

	// ����С��������ʾλΪ1������ʾС����
	code = (m_bPoint && pos == 1) ? code + 128 : code;

	uint8_t tranMsg[] = { (pos == m_nCursorPos && blank) ? DigUtils::GetBlankCode() : code,
			(uint8_t) ~(0x1 << (3 - pos)) };

	stSpiSetting[0].tx_buf = (uint64_t) tranMsg;

	ret = ioctl(m_fdSPI, SPI_IOC_MESSAGE(1), &stSpiSetting[0]);
	if (ret == 1)
		handle_error("cannot receive spi message!");
	ret = ioctl(m_fdRefresh, SET_GPIO_LOW);
//	usleep(100);
	ret = ioctl(m_fdRefresh, SET_GPIO_HIGHT);
}

inline void CDigitronSPI::SetCursorPos(int pos) {
	if ((pos >= 0 && pos < 4) || pos == -1)
		m_nCursorPos = pos;
}

inline void CDigitronSPI::CursorLeftShift() {
	if (m_nCursorPos == -1)
		return;
	m_nCursorPos++;
	m_nCursorPos %= 4;
}

inline void CDigitronSPI::CursorRightShift() {
	if (m_nCursorPos == -1)
		return;
	m_nCursorPos += 3;
	m_nCursorPos %= 4;
}

inline void CDigitronSPI::CursorDataIncrease() {
	if (m_nCursorPos == -1)
		return;
	int index = DigUtils::GetIndexOfCode(m_nDisCharCode[m_nCursorPos]);
	if (index < 0 || index > 9)
		return;
	index = (index + 1) % 10;
	m_nDisCharCode[m_nCursorPos] = Dig_Figure[index];
}

inline void CDigitronSPI::CursorDataDecrease() {
	if (m_nCursorPos == -1)
		return;
	int index = DigUtils::GetIndexOfCode(m_nDisCharCode[m_nCursorPos]);
	if (index < 0 || index > 9)
		return;
	index = (index + 9) % 10;
	m_nDisCharCode[m_nCursorPos] = Dig_Figure[index];
}

inline void CDigitronSPI::SetPointEnable(bool b) {
	m_bPoint = b;
}

inline void CDigitronSPI::GetDegree(int* degree) {
	if (!m_bPoint)
		return;
	int sum = 0;
	sum += DigUtils::GetIndexOfCode(m_nDisCharCode[0]);
	sum += DigUtils::GetIndexOfCode(m_nDisCharCode[1]) * 10;
	sum += DigUtils::GetIndexOfCode(m_nDisCharCode[2]) * 100;
	sum += DigUtils::GetIndexOfCode(m_nDisCharCode[3]) * 1000;
	*degree = sum;
}

#endif /* CDIGITRONSPI_H_ */
