/**
 * File name: CKeyScan.h
 * Author: JoshuaWong
 * Email: 1034316376@qq.com
 * Version: 1.0
 * Created on: 2016��11��30�� ����8:02:00
 * Description: 
 */

#ifndef CKEYSCAN_H_
#define CKEYSCAN_H_

#include <fcntl.h>
#include <sys/ioctl.h>
#include "common.h"

#define DEV_ROW1 "/dev/gpio-P1.24-row1"
#define DEV_ROW2 "/dev/gpio-P1.25-row2"
#define DEV_ROW3 "/dev/gpio-P1.26-row3"
#define DEV_ROW4 "/dev/gpio-P1.27-row4"
#define DEV_COL1 "/dev/gpio-P3.28-line1"
#define DEV_COL2 "/dev/gpio-P3.30-line2"
#define DEV_STEP_L "/dev/gpio-DUTX"
#define DEV_STEP_R "/dev/gpio-DURX"

#define COUNT_COL 2
#define COUNT_ROW 4

#define KEY_CODE_NONE 0
#define KEY_CODE_UNKNOWN -1
#define CLICK_THRESHOLD 50		//���̰��ٽ�ֵ

typedef void (*pOnClick)(uint8_t);

const char *DEVS[] = { DEV_ROW1, DEV_ROW2, DEV_ROW3, DEV_ROW4, DEV_COL1, DEV_COL2 };
const char *DEV_COLS[] = { DEV_COL1, DEV_COL2 };
const char *DEV_ROWS[] = { DEV_ROW1, DEV_ROW2, DEV_ROW3, DEV_ROW4 };

enum ClickType {
	SHORT_CLICK, LONG_CLICK,
};

/**
 * ����������
 */
class OnLongClickListener {
protected:
	~OnLongClickListener();

public:
	/**
	 * �����¼�������
	 * @param key:��ֵ
	 */
	virtual void OnLongClick(uint8_t key) = 0;
};

class CKeyScan {
public:
	CKeyScan();
	~CKeyScan();

	void test();

	/**
	 * ��ʼ���а���ɨ��
	 * @param ��
	 */
	void StartScan();

	/**
	 * ���ð�������
	 * @param onClick:�����¼�������ָ��
	 */
	void SetOnClickListener(pOnClick onClick);
	/**
	 * ���ó�������
	 * @param listener:����������
	 */
	void SetOnLongClickListener(OnLongClickListener *listener);

private:
	// ���豸�ļ���־
	int m_fdCols[COUNT_COL];
	// ���豸�ļ���־
	int m_fdRows[COUNT_ROW];
	// ����ת
	int m_fdStepL, m_fdStepR;
	//��ֵ
	//uint8_t m_nKeyValue;
	//�̰�������ָ��
	pOnClick onClick;
	//����������
	OnLongClickListener *listener;

	//����������־	�м�����˲���0
	uint16_t m_nTrg;
	//�������³�����־	�м�������״̬����Ϊ1���޼�����ʱ����Ϊ0
	uint16_t m_nCont;
	//�����ͷŴ�����־	�м�����˲���0
	uint16_t m_nRelease;
	/*
	 * ��һ������־ֵ	���ڲ�׽������־�������غ��½��أ�
	 * ������Ϊ�����׸���������˲�䣨����ͬʱ���¶�����������
	 * �½���Ϊ�����ͷ�˲��
	 */
//	uint8_t m_nLastCont;
	/**
	 * �������豸��ƽ
	 * @param cmd=SET_GPIO_HIGHT	���豸ȫ���ø�
	 * 		  cmd=SET_GPIO_LOW		���豸ȫ���õ�
	 * @return
	 */
	void __SetColsValue(CMD cmd);
	/**
	 * ��ȡ�������豸��ƽ
	 * @return �������豸��ƽֵ
	 */
	uint8_t __GetColsValue();
	/**
	 * �������豸��ƽ
	 * @param cmd=SET_GPIO_HIGHT	���豸ȫ���ø�
	 * 		  cmd=SET_GPIO_LOW		���豸ȫ���õ�
	 */
	void __SetRowsValue(CMD cmd);
	/**
	 * ��ȡ�������豸��ƽ
	 * @return �������豸��ƽֵ
	 */
	uint8_t __GetRowsValue();

	/**
	 * ��ȡ����ת������ƽ
	 */
	uint8_t __GetStepLValue();
	uint8_t __GetStepRValue();

	/**
	 * ���豸col�õͣ������ø�
	 * @param col:��λ�����
	 */
	void __ClearCol(int col);

	/**
	 * �����Ի�ȡ��ֵ
	 * @param type:��������	keyCode:����
	 * @return ��ֵ
	 */
	uint8_t __GetKeyValue(ClickType type, uint16_t keyCode);

	void __OnClick(uint8_t key);
	void __OnLongClick(uint8_t key);

	/**
	 * ���°�����־��m_nTrg��m_nCont��
	 */
	void __KeyRead();
	/**
	 * ��������
	 */
	void __KeyProc();
};

inline CKeyScan::CKeyScan() :
		m_nTrg(0x0), m_nCont(0x0), m_nRelease(0x0) {
	int i;
//	m_nLastCont = 0x0;
	//m_nKeyValue = KEY_CODE_NONE;
	onClick = NULL;
	listener = NULL;

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

	printf("CKeyScan Object destoryed!\n");
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
	while (1) {
		__KeyRead();
		__KeyProc();
		usleep(20 * 1000);
	}
}

inline void CKeyScan::SetOnClickListener(pOnClick onClick) {
	this->onClick = onClick;
}

inline void CKeyScan::SetOnLongClickListener(OnLongClickListener *listener) {
	this->listener = listener;
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

inline void CKeyScan::__ClearCol(int col) {
	int ret, i;

	for (i = 0; i < COUNT_COL; i++) {
		ret = ioctl(m_fdCols[i], i == col ? SET_GPIO_LOW : SET_GPIO_HIGHT);
		if (ret < 0)
			handle_err_log("%s %s", DEV_COLS[i], "ioctl");
	}
}

inline uint8_t CKeyScan::__GetKeyValue(ClickType type, uint16_t keyCode) {
	uint8_t keyValue = KEY_CODE_UNKNOWN;
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

inline void CKeyScan::__OnClick(uint8_t key) {
	printf("OnClick:%d\n", key);
	if (onClick != NULL)
		onClick(key);
}

inline void CKeyScan::__OnLongClick(uint8_t key) {
	printf("OnLongClick:%d\n", key);
	if (listener != NULL)
		listener->OnLongClick(key);
}

inline void CKeyScan::__KeyProc() {
	static uint16_t keyCode = KEY_CODE_NONE;
	static int cntFlag = 0;

	if (m_nTrg) {
		if (keyCode == KEY_CODE_NONE)	//���ζ��ͬʱ����
			keyCode = m_nTrg;
		else
			keyCode = KEY_CODE_NONE;
		cntFlag = 0;
	}

	if (m_nCont & keyCode) {
		cntFlag++;
		if (cntFlag == CLICK_THRESHOLD) {	//�жϳ����������г�������1sΪ����
			__OnLongClick(__GetKeyValue(LONG_CLICK, keyCode));
		}
	}

	// ��׽�����ɿ�ʱ�̣����ж��Ƿ�Ϊ�½���
//	if ((~m_nCont) & (m_nLastCont ^ m_nCont)) {
//		if (cntFlag > 1 && cntFlag < CLICK_THRESHOLD)	//�ж϶̰��������ж̰�����
//			__OnClick(__GetKeyValue(SHORT_CLICK, keyCode));
//		keyCode = KEY_CODE_NONE;
//	}

	if (m_nRelease) {
		if (cntFlag > 1 && cntFlag < CLICK_THRESHOLD) {	//�ж϶̰��������ж̰�����
			log("keycode: %d", keyCode);
			__OnClick(__GetKeyValue(SHORT_CLICK, keyCode));
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

	// ��ȡ����ת����ֵ
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
