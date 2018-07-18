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
#include <fstream>
#include <iostream>
#include <unistd.h>
#include <stdint.h>
#include <errno.h>
#include <string.h>
#include <string>
#include <vector>
#include <sstream>
#include <termios.h>
#include <fcntl.h>

using namespace std;
#define uchar unsigned char
#define PRE_DEGREE_COUNT 10

// Socket
#define MSG_RECV_HEAD	0x2424
#define MSG_SEND_HEAD	0x2929
#define MSG_END			0x0d
#define MSG_HEAD_SIZE	2
#define MSG_CMD_SIZE	1
#define MSG_END_SIZE	1
#define MSG_CRC_SIZE	2

#define ADDRESS 0x01

#define SOCKET_MSG_FORMAT_SIZE (MSG_HEAD_SIZE + MSG_CMD_SIZE + MSG_CRC_SIZE + MSG_END_SIZE)

#define BUFFER_SIZE 1024
#define MSG_CACHE_SIZE			512
#define SOCKET_MSG_DATA_SIZE	32
#define SOCKET_MSG_SIZE		(SOCKET_MSG_DATA_SIZE + SOCKET_MSG_FORMAT_SIZE)

// ���Ҳ���ö�٣����ڲ���ʱ�жϲ���֡�ṹ���ĸ���λ
typedef enum {
	SEARCH_HEAD, SEARCH_CMD, SEARCH_END
} SearchStrategy;

// NCU->ADU	����ָ��
typedef enum {
	CMD_HEARTBEAT = 0x01,			// ����ָ��
	CMD_ONLINE = 0x02,				// ����ָ��
	CMD_ROTATE_TO_DEGREE = 0x03,	// ��λ����ָ��
	CMD_ROTATE_TO_POINT = 0x05,		// �������ָ��
	CMD_CORRECT = 0x06,				// �Ƕ�У׼ָ��
	CMD_ROTATE_FWD = 0x07,			// ˳ʱ��㶯����
	CMD_ROTATE_REV = 0x08,			// ��ʱ��㶯����
	CMD_STOP = 0x0a,				// ����ͣ��ָ��
	CMD_RUN_PARAM_QUERY = 0x0b,		// ����������ѯ
	CMD_CORRECT_QUERY = 0x0c,		// У׼�ǶȲ�ѯ
	CMD_TEN_POINT_QUERY = 0x0d,		// 10������ǶȲ�ѯ
	CMD_TEN_POINT_SET = 0x0f,		// 10������Ƕ�����
	CMD_ADU_REBOOT = 0x10,			// ADUreboot
	CMD_RUN_DEGREE = 0x11,			// ��ת��ԽǶ�

	CMD_FPGA_BACK = 0x52,			//FPGA->ADU
} DownstreamCmd;

// ADU->NCU	����ָ��
typedef enum {
	CMD_BACK_HEARTBEAT = 0x01,		// �����ظ�ָ��
	CMD_BACK_ONLINE = 0x02,			// �����ظ�ָ��
	CMD_BACK_RUN_PARAM = 0x03,		// �豸���������ظ�ָ��
	CMD_BACK_TEN_POINT = 0x05,		// �豸10������ǶȲ�ѯ/���ûظ�ָ��
	CMD_BACK_CORRECT = 0x06,		// �豸(ADU)�Ƕ�У׼�ظ�ָ��
	CMD_BACK_CORRECT_QUERY = 0x07,	// �豸У׼�ǶȲ�ѯ�ظ�ָ��

	CMD_FPGA_SEND = 0x51,			//ADU->FPGA
} UpstreamCmd;

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
//ʱ������
time_t now;
struct tm *timenow;
/* ������깩���ش�����ĺ���ʹ��*/
#define handle_error_en(en, msg) do{errno=en; perror(msg); exit(EXIT_FAILURE);} while(0)
/* �������*/
#define handle_error(msg) do{perror(msg); exit(EXIT_FAILURE);} while(0)
#define handle_err_log(x, args...) do{fprintf(stderr, x"\n", ##args); exit(EXIT_FAILURE);}while(0)
#define log(x, args...) do{printf("file:%s function:%s line:%d "x"\n", __FILE__, __FUNCTION__, __LINE__, args);}while(0)

// DELAY
#define KEY_SCAN_DELAY_US 20*1000	// ����ɨ����
#define DIGITRON_DELAY_US 3*1000	// ����ܶ�̬��ʾ���
#define DATA_PARSE_DELAY_US 50*1000		// Socket Server���ݽ������

#endif /* COMMON_H_ */
