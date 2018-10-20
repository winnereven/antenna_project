/**
 * File name: CKeyScan.h
 * Author: JoshuaWong
 * Email: 1034316376@qq.com
 * Version: 1.0
 * Created on: 2016年11月30日 下午8:02:00
 * Description: 
 */

#ifndef CKEYSCAN_H_
#define CKEYSCAN_H_

#include <fcntl.h>
#include <sys/ioctl.h>
#include "../common.h"
#include "IOnKeyClickListener.h"

#define DEV_ROW1 "/dev/gpio-P2.12"
#define DEV_ROW2 "/dev/gpio-P2.13"
#define DEV_ROW3 "/dev/gpio-P2.14"
#define DEV_ROW4 "/dev/gpio-P2.15"
#define DEV_COL1 "/dev/gpio-P1.17"
#define DEV_COL2 "/dev/gpio-P1.18"
#define DEV_STEP_L "/dev/gpio-P2.7"
#define DEV_STEP_R "/dev/gpio-P2.6"
#define DEV_CTL_MODE "/dev/gpio-UTX0"
#define DEV_STOP "/dev/gpio-URX0"

#define COUNT_COL 2
#define COUNT_ROW 4

#define KEY_CODE_NONE 0
#define KEY_CODE_UNKNOWN -1
#define CLICK_THRESHOLD 50		//长短按临界值

const char *DEVS[] = { DEV_ROW1, DEV_ROW2, DEV_ROW3, DEV_ROW4, DEV_COL1, DEV_COL2 };
const char *DEV_COLS[] = { DEV_COL1, DEV_COL2 };
const char *DEV_ROWS[] = { DEV_ROW1, DEV_ROW2, DEV_ROW3, DEV_ROW4 };

enum ClickType {
	SHORT_CLICK, LONG_CLICK,
};

class CKeyScan {
public:
	CKeyScan();
	~CKeyScan();

	void test();

	/**
	 * 开始进行按键扫描
	 * @param 无
	 */
	void StartScan();

	/**
	 * 设置按键监听
	 * @param onClick:按键事件处理函数指针
	 */
	void SetOnClickListener(IOnKeyClickListener* listener);

	bool GetCtlMode();

private:
	// 行设备文件标志
	int m_fdCols[COUNT_COL];
	// 列设备文件标志
	int m_fdRows[COUNT_ROW];
	// 正反转
	int m_fdStepL, m_fdStepR;
	int m_fdStop;
	int m_fdCtlMode;
	//键值
	//uint8_t m_nKeyValue;
	// 短按处理函数指针
	IOnKeyClickListener* mKeyClickListener;

	// 是否为本控状态，否则为远控
	bool isLocalCtlMode;

	//按键触发标志	有键按下瞬间非0
	uint16_t m_nTrg;
	//按键按下持续标志	有键处理按下状态持续为1，无键按下时持续为0
	uint16_t m_nCont;
	//按键释放触发标志	有键按下瞬间非0
	uint16_t m_nRelease;
	/*
	 * 上一持续标志值	用于捕捉持续标志的上升沿和下降沿，
	 * 上升沿为按键首个按键按下瞬间（存在同时按下多个按键情况）
	 * 下降沿为按键释放瞬间
	 */
	//	uint8_t m_nLastCont;
	/**
	 * 设置行设备电平
	 * @param cmd=SET_GPIO_HIGHT	行设备全部置高
	 * 		  cmd=SET_GPIO_LOW		行设备全部置低
	 * @return
	 */
	void __SetColsValue(CMD cmd);
	/**
	 * 获取所有行设备电平
	 * @return 所有行设备电平值
	 */
	uint8_t __GetColsValue();
	/**
	 * 设置列设备电平
	 * @param cmd=SET_GPIO_HIGHT	列设备全部置高
	 * 		  cmd=SET_GPIO_LOW		列设备全部置低
	 */
	void __SetRowsValue(CMD cmd);
	/**
	 * 获取所有列设备电平
	 * @return 所有列设备电平值
	 */
	uint8_t __GetRowsValue();

	// 获取正反转按键电平
	uint8_t __GetStepLValue();
	uint8_t __GetStepRValue();

	// 获取本控/远控状态
	bool __GetCtlMode();
	//获取停止状态
	bool __GetStop();
	/**
	 * 行设备col置低，其余置高
	 * @param col:清位行序号
	 */
	void __ClearCol(int col);

	/**
	 * 解码以获取键值
	 * @param type:按键类型	keyCode:键码
	 * @return 键值
	 */
	int __GetKeyValue(ClickType type, uint16_t keyCode);

	/**
	 * 更新按键标志（m_nTrg和m_nCont）
	 */
	void __KeyRead();
	/**
	 * 按键处理
	 */
	void __KeyProc();
};

inline CKeyScan::CKeyScan() :
		m_nTrg(0x0), m_nCont(0x0), m_nRelease(0x0) {
	int i;
	//m_nLastCont = 0x0;
	//m_nKeyValue = KEY_CODE_NONE;
	mKeyClickListener = NULL;

	printf("CKeyScan Object created!\n");
	for (i = 0; i < COUNT_COL; i++) {
		m_fdCols[i] = open(DEV_COLS[i], O_RDWR);
		if (m_fdCols[i] < 0)
			handle_err_log("%s %s", DEV_COLS[i], "open");
	}
	for (i = 0; i < COUNT_ROW; i++) {
		m_fdRows[i] = open(DEV_ROWS[i], O_RDWR);
		if (m_fdRows[i] < 0)
			handle_err_log("%s %s", DEV_ROWS[i], "open");
	}
	m_fdStepL = open(DEV_STEP_L, O_RDWR);
	if (m_fdStepL < 0)
		handle_err_log("%s %s", DEV_STEP_L, "open");
	m_fdStepR = open(DEV_STEP_R, O_RDWR);
	if (m_fdStepR < 0)
		handle_err_log("%s %s", DEV_STEP_R, "open");
	m_fdCtlMode = open(DEV_CTL_MODE, O_RDWR);
	if (m_fdCtlMode < 0)
		handle_err_log("%s %s", DEV_CTL_MODE, "open");
	m_fdStop = open(DEV_STOP, O_RDWR);
	if (m_fdStop < 0)
		handle_err_log("%s %s", DEV_STOP, "open");
	isLocalCtlMode = __GetCtlMode();
}

inline CKeyScan::~CKeyScan() {
	int i, ret;

	for (i = 0; i < COUNT_COL; i++) {
		ret = close(m_fdCols[i]);
		if (ret)
			handle_err_log("%s %s", DEV_COLS[i], "close");
	}
	for (i = 0; i < COUNT_ROW; i++) {
		ret = close(m_fdRows[i]);
		if (ret)
			handle_err_log("%s %s", DEV_ROWS[i], "close");
	}

	ret = close(m_fdStepL);
	if (ret)
		handle_err_log("%s %s", DEV_STEP_L, "close");
	ret = close(m_fdStepR);
	if (ret)
		handle_err_log("%s %s", DEV_STEP_R, "close");
	ret = close(m_fdCtlMode);
	if (ret)
		handle_err_log("%s %s", DEV_CTL_MODE, "close");
	ret = close(m_fdStop);
	if (ret)
		handle_err_log("%s %s", DEV_CTL_MODE, "close");

	printf("CKeyScan Object destroyed!\n");
}

inline void CKeyScan::test() {
	int ret;
	for (int i = 0; i < COUNT_COL; i++) {
		ret = ioctl(m_fdCols[i], SET_GPIO_LOW);
		if (ret < 0)
			handle_err_log("%s %s", DEV_COLS[i], "ioctl");
	}

	for (int i = 0; i < COUNT_ROW; i++) {
		ret = ioctl(m_fdRows[i], SET_GPIO_HIGHT);
		if (ret < 0)
			handle_err_log("%s %s", DEV_ROWS[i], "ioctl");
	}
}

inline void CKeyScan::StartScan() {
	printf("CKeyScan::StartScan()\n");
	bool newCtlMode;
	while (1) {
		if (isLocalCtlMode) {
			__KeyRead();
			__KeyProc();
		}

		newCtlMode = __GetCtlMode();
		if (isLocalCtlMode != newCtlMode && mKeyClickListener != NULL) {
			isLocalCtlMode = newCtlMode;
			mKeyClickListener->onCtlModeChanged(isLocalCtlMode);
		}
		if(__GetStop())
		{
			mKeyClickListener->onKeyClick(KEY_STOP);
			if(__GetStop()==0)
				mKeyClickListener->onKeyClick(KEY_USTOP);
		}
		usleep(KEY_SCAN_DELAY_US);
	}
}

inline void CKeyScan::SetOnClickListener(IOnKeyClickListener* listener) {
	this->mKeyClickListener = listener;
	// 启动时进行一次本远控判断
	isLocalCtlMode = __GetCtlMode();
	if (mKeyClickListener != NULL)
		mKeyClickListener->onCtlModeChanged(isLocalCtlMode);
}

inline bool CKeyScan::GetCtlMode() {
	return isLocalCtlMode;
}

inline void CKeyScan::__SetColsValue(CMD cmd) {
	int i, ret;
	for (i = 0; i < COUNT_COL; i++) {
		ret = ioctl(m_fdCols[i], cmd);
		if (ret < 0)
			handle_err_log("%s %s", DEV_COLS[i], "ioctl");
	}
}

inline uint8_t CKeyScan::__GetColsValue() {
	int ret;
	uint8_t value = 0x0;
	uint8_t buf;
	int i;
	for (i = 0; i < COUNT_COL; i++) {
		ret = read(m_fdCols[i], &buf, 1);
		if (ret < 0)
			handle_err_log("%s %s", DEV_COLS[i], "read");
		value |= ((buf & 0x1) << i);
	}
	return value;
}

inline void CKeyScan::__SetRowsValue(CMD cmd) {
	int i, ret;
	for (i = 0; i < COUNT_ROW; i++) {
		ret = ioctl(m_fdRows[i], cmd);
		if (ret < 0)
			handle_err_log("%s %s", DEV_ROWS[i], "ioctl");
	}
}

inline uint8_t CKeyScan::__GetRowsValue() {
	int ret;
	uint8_t value = 0x0;
	uint8_t buf;
	int i;
	for (i = 0; i < COUNT_ROW; i++) {
		ret = read(m_fdRows[i], &buf, 1);
		if (ret < 0)
			handle_err_log("%s %s", DEV_ROWS[i], "read");
		value |= ((buf & 0x1) << i);
	}
	return value;
}

inline uint8_t CKeyScan::__GetStepLValue() {
	int ret;
	uint8_t buf;
	ret = read(m_fdStepL, &buf, 1);
	if (ret < 0)
		handle_err_log("%s %s", DEV_STEP_L, "read");
	return buf;
}

inline uint8_t CKeyScan::__GetStepRValue() {
	int ret;
	uint8_t buf;
	ret = read(m_fdStepR, &buf, 1);
	if (ret < 0)
		handle_err_log("%s %s", DEV_STEP_R, "read");
	return buf;
}

inline bool CKeyScan::__GetCtlMode() {
	int ret;
	uint8_t buf;
	ret = read(m_fdCtlMode, &buf, 1);
	if (ret < 0)
		handle_err_log("%s %s", DEV_CTL_MODE, "read");
	return buf == 0; // 本控为0，远控为1
}
inline bool CKeyScan::__GetStop() {
	int ret;
	uint8_t buf;
	ret = read(m_fdStop, &buf, 1);
	if (ret < 0)
		handle_err_log("%s %s", DEV_STOP, "read");
	return buf; // 不按为0，按下为1
}
inline void CKeyScan::__ClearCol(int col) {
	int ret, i;

	for (i = 0; i < COUNT_COL; i++) {
		ret = ioctl(m_fdCols[i], i == col ? SET_GPIO_LOW : SET_GPIO_HIGHT);
		if (ret < 0)
			handle_err_log("%s %s", DEV_COLS[i], "ioctl");
	}
}

inline int CKeyScan::__GetKeyValue(ClickType type, uint16_t keyCode) {
	int keyValue = KEY_CODE_UNKNOWN;
	int i;


	switch (type) {
	case SHORT_CLICK:
		for (i = 0; i < 10; i++)
			if (!(keyCode ^ (1 << i)))
				keyValue = i + 1;
		break;
	case LONG_CLICK:
		for (i = 0; i < 10; i++)
			if (!(keyCode ^ (1 << i)))
				keyValue = i + 11;
		break;
	}

	return keyValue;
}

inline void CKeyScan::__KeyProc() {
	static uint16_t keyCode = KEY_CODE_NONE;
	static int cntFlag = 0;

	if (m_nTrg) {
		if (keyCode == KEY_CODE_NONE)	//屏蔽多键同时按下
			keyCode = m_nTrg;
		else
			keyCode = KEY_CODE_NONE;
//		printf("cntFlag=%d\n",cntFlag);
		cntFlag = 0;
	}

	if (m_nCont & keyCode) {
		cntFlag++;
		if (cntFlag == CLICK_THRESHOLD) {	//判断长按，并进行长按处理，1s为长按
			log("long keycode: %d", keyCode);
			if (mKeyClickListener != NULL) {
				system("echo 1 > /sys/class/leds/beep/brightness");
				mKeyClickListener->onKeyClick(__GetKeyValue(LONG_CLICK, keyCode));
				system("echo 0 > /sys/class/leds/beep/brightness");

			}
		}
	}

// 捕捉按键松开时刻，即判断是否为下降沿
//	if ((~m_nCont) & (m_nLastCont ^ m_nCont)) {
//		if (cntFlag > 1 && cntFlag < CLICK_THRESHOLD)	//判断短按，并进行短按处理
//			__OnClick(__GetKeyValue(SHORT_CLICK, keyCode));
//		keyCode = KEY_CODE_NONE;
//	}

	if (m_nRelease) {
		if (cntFlag > 1 && cntFlag < CLICK_THRESHOLD) {	//判断短按，并进行短按处理
			log("short keycode: %d", keyCode);
			if (mKeyClickListener != NULL) {
				system("echo 1 > /sys/class/leds/beep/brightness");
				mKeyClickListener->onKeyClick(__GetKeyValue(SHORT_CLICK, keyCode));
				system("echo 0 > /sys/class/leds/beep/brightness");
			}
		}
		keyCode = KEY_CODE_NONE;
	}
}

inline void CKeyScan::__KeyRead() {
	uint16_t readData = 0x0;
	uint8_t valueRows;
	uint16_t valueStepL;
	uint16_t valueStepR;
	__ClearCol(0);
	usleep(1000);
	valueRows = __GetRowsValue();
	readData |= valueRows;
	__ClearCol(1);
	usleep(1000);
	valueRows = __GetRowsValue();
	readData |= (valueRows << 4);

	// 读取正反转按键值
	valueStepL = __GetStepLValue();
	readData |= (valueStepL << 8);
	valueStepR = __GetStepRValue();
	readData |= (valueStepR << 9);

	readData ^= 0x03ff;
	//	m_nLastCont = m_nCont;
	m_nTrg = readData & (readData ^ m_nCont);
	m_nRelease = (readData ^ m_nCont) ^ m_nTrg;
	m_nCont = readData;
}

#endif /* CKEYSCAN_H_ */
