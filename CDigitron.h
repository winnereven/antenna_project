/**
 * Digitron.h
 * Author: JoshuaWong JoshuaWong
 * Email: 1034316376@qq.com
 * Created on: 2016年11月28日 上午10:59:52
 * Description: 
 */

#ifndef CDIGITRON_H_
#define CDIGITRON_H_

#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "common.h"
#include "Linux/gpio.h"

#define NUM_LEDS 4
#define NUM_LED_BITS 8

uint8_t Led_Figure[] = 	//不带小数点的段码
		{ 0x3f, //a+b+c+d+e+f,     "0"
				0x06, //b+c,             "1"
				0x5b, //a+b+g+e+d,       "2"
				0x4f, //a+b+g+c+d,       "3"
				0x66, //f+g+b+c,         "4"
				0x6d, //a+f+g+c+d,       "5"
				0x7d, //a+f+g+e+c+d,     "6"
				0x07, //a+b+c,           "7"
				0x7f, //a+b+c+d+e+f+g,   "8"
				0x6f, //a+b+c+d+f+g,     "9"
				0x00, //                 全暗即" "
				0x39, //d+e+g,           "C"
				0x40, //g                "-"
				0x5f, //a+b+c+d+e+g      “a”
				0x38, //d+e+f            "L"
				0x71, //a+g+e+f          "F"
				0x6e, //b+c+d+f+g        "y"
				0x73  //a+b+e+f+g        "p"
		};

const char *gDIG_BITS[] = { "/dev/gpio-P3.27", "/dev/gpio-P3.26", "/dev/gpio-P3.21",
		"/dev/gpio-P3.20", "/dev/gpio-P2.15", "/dev/gpio-P2.14", "/dev/gpio-P2.13",
		"/dev/gpio-P2.12" };
const char *gDIGS_EN[] = { "/dev/gpio-P2.7", "/dev/gpio-P2.6", "/dev/gpio-P2.5", "/dev/gpio-P2.4" };
const int nDigSleep = 1000;	//数码管显示间隔1000*4us

class CDigitron {
private:
	int m_gDigBits[NUM_LED_BITS];
	int m_gDigsEn[NUM_LEDS];

	void DigOn(int pos);

public:
	CDigitron();
	~CDigitron();

	void DisplayNum(int num);
	void DisplayBit(int num, int pos);
	void DisplayBit(int num, int pos, bool withDot);
};

inline CDigitron::CDigitron() {
	int i;

	for (i = 0; i < NUM_LED_BITS; i++) {
		m_gDigBits[i] = open(gDIG_BITS[i], O_RDWR);
		if (m_gDigBits[i] < 0)
			handle_error("open digitron bit failed");
	}

	for (i = 0; i < NUM_LEDS; i++) {
		m_gDigsEn[i] = open(gDIGS_EN[i], O_RDWR);
		if (m_gDigsEn[i] < 0)
			handle_error("open digitron port failed");
	}
}

inline CDigitron::~CDigitron() {
}

/**
 * 显示数字
 * @param num:将显示数据值
 * @return
 */
inline void CDigitron::DisplayNum(int num) {
	int i;
	int arr[4];

	for (i = 0; i < 4; i++) {
		arr[3 - i] = num % 10;
		num /= 10;
	}

	i = 0;
	while (1) {
		DisplayBit(arr[i], i);
		i++;
		i %= 4;
		usleep(1000);
	}
}

/**
 * 显示单位数码管（不带小数点）
 * @param num:显示字符	pos:显示位置
 * @return
 */
inline void CDigitron::DisplayBit(int num, int pos) {
	DisplayBit(num, pos, false);
}

/**
 * 显示单位数码管
 * @param num:显示字符	pos:显示位置	 withDot:是否显示小数点
 * @return
 */
inline void CDigitron::DisplayBit(int num, int pos, bool withDot) {
	int ret;

	DigOn(-1);
	for (int i = 0; i < NUM_LED_BITS; i++) {
		uint8_t buf = (Led_Figure[num] >> i) & 0x1;
		if (buf)
			ret = ioctl(m_gDigBits[i], SET_GPIO_HIGHT);
		else
			ret = ioctl(m_gDigBits[i], SET_GPIO_LOW);
		if (ret < 0)
			handle_error_en(ret, "set digitron port failed");
	}
	DigOn(pos);
}

/**
 * 选择数码管
 * @param pos:点亮数码管序号	为-1时全灭
 * @return
 */
inline void CDigitron::DigOn(int pos) {
	for (int i = 0; i < NUM_LEDS; i++) {
		int ret = ioctl(m_gDigsEn[i], i == pos ? SET_GPIO_LOW : SET_GPIO_HIGHT);
		if (ret < 0)
			handle_error_en(ret, "set digitron bit failed");
	}
}

#endif /* CDIGITRON_H_ */
