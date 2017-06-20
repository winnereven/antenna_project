/**
 * File name: common.h
 * Author: JoshuaWong
 * Email: 1034316376@qq.com
 * Version: 1.0
 * Created on: 2016��11��29�� ����5:22:11
 * Description: 
 */

#ifndef COMMON_H_
#define COMMON_H_

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <unistd.h>
#include <stdint.h>
#include <errno.h>
#include <string>

#define uchar unsigned char

// �������
enum MatrixKey {
	KEY_BACK = 1,	// ����
	KEY_CORRECT,	// У׼
	KEY_UP,			// ��
	KEY_MODE,		// ģʽ
	KEY_OK,			// ȷ��
	KEY_RIGTH,		// ��
	KEY_DOWN,		// ��
	KEY_LEFT,		// ��
	KEY_MOTOR_REV,	// �����ת
	KEY_MOTOR_FWD,	// �����ת

	KEY_BACK_L,		// ����(����)
	KEY_CORRECT_L,	// У׼(����)
	KEY_UP_L,		// ��(����)
	KEY_MODE_L,		// ģʽ(����)
	KEY_OK_L,		// ȷ��(����)
	KEY_RIGTH_L,	// ��(����)
	KEY_DOWN_L,		// ��(����)
	KEY_LEFT_L,		// ��(����)
	KEY_MOTOR_REV_L,	// �����ת(����)
	KEY_MOTOR_FWD_L	// �����ת(����)
};

// ��ʾģʽ
enum DisplayMode {
	FUNCTION_SELECT = 9,	// ����ѡ��
	DEGREE_SETTING,			// �Ƕ�����
	DEGREE_SHOW,			// �Ƕ���ʾ
	PRE_DEGREE_SELECT,		// Ԥ��Ƕ�ѡ��
};

using namespace std;

/* ������깩���ش�����ĺ���ʹ��*/
#define handle_error_en(en, msg) do{errno=en; perror(msg); exit(EXIT_FAILURE);} while(0)
/* �������*/
#define handle_error(msg) do{perror(msg); exit(EXIT_FAILURE);} while(0)
#define handle_err_log(x, args...) do{fprintf(stderr, x"\n", ##args); exit(EXIT_FAILURE);}while(0)
#define log(x, args...) do{printf("file:%s function:%s line:%d "x"\n", __FILE__, __FUNCTION__, __LINE__, args);}while(0)

#endif /* COMMON_H_ */
