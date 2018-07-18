/**
 * File name: common.h
 * Author: JoshuaWong
 * Email: 1034316376@qq.com
 * Version: 1.0
 * Created on: 2016年11月29日 下午5:22:11
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

// 查找策略枚举，用于查找时判断查找帧结构的哪个部位
typedef enum {
	SEARCH_HEAD, SEARCH_CMD, SEARCH_END
} SearchStrategy;

// NCU->ADU	下行指令
typedef enum {
	CMD_HEARTBEAT = 0x01,			// 心跳指令
	CMD_ONLINE = 0x02,				// 联机指令
	CMD_ROTATE_TO_DEGREE = 0x03,	// 定位控制指令
	CMD_ROTATE_TO_POINT = 0x05,		// 定点控制指令
	CMD_CORRECT = 0x06,				// 角度校准指令
	CMD_ROTATE_FWD = 0x07,			// 顺时针点动控制
	CMD_ROTATE_REV = 0x08,			// 逆时针点动控制
	CMD_STOP = 0x0a,				// 天线停车指令
	CMD_RUN_PARAM_QUERY = 0x0b,		// 工况参数查询
	CMD_CORRECT_QUERY = 0x0c,		// 校准角度查询
	CMD_TEN_POINT_QUERY = 0x0d,		// 10个定点角度查询
	CMD_TEN_POINT_SET = 0x0f,		// 10个定点角度设置
	CMD_ADU_REBOOT = 0x10,			// ADUreboot
	CMD_RUN_DEGREE = 0x11,			// 旋转相对角度

	CMD_FPGA_BACK = 0x52,			//FPGA->ADU
} DownstreamCmd;

// ADU->NCU	上行指令
typedef enum {
	CMD_BACK_HEARTBEAT = 0x01,		// 心跳回复指令
	CMD_BACK_ONLINE = 0x02,			// 联机回复指令
	CMD_BACK_RUN_PARAM = 0x03,		// 设备工况参数回复指令
	CMD_BACK_TEN_POINT = 0x05,		// 设备10个定点角度查询/设置回复指令
	CMD_BACK_CORRECT = 0x06,		// 设备(ADU)角度校准回复指令
	CMD_BACK_CORRECT_QUERY = 0x07,	// 设备校准角度查询回复指令

	CMD_FPGA_SEND = 0x51,			//ADU->FPGA
} UpstreamCmd;

// 矩阵键盘
enum MatrixKey {
	KEY_BACK = 1,	// 返回
	KEY_CORRECT,	// 校准
	KEY_UP,			// 上
	KEY_MODE,		// 模式
	KEY_OK,			// 确认
	KEY_RIGTH,		// 右
	KEY_DOWN,		// 下
	KEY_LEFT,		// 左
	KEY_MOTOR_REV,	// 电机反转
	KEY_MOTOR_FWD,	// 电机正转

	KEY_BACK_L,		// 返回(长按)
	KEY_CORRECT_L,	// 校准(长按)
	KEY_UP_L,		// 上(长按)
	KEY_MODE_L,		// 模式(长按)
	KEY_OK_L,		// 确认(长按)
	KEY_RIGTH_L,	// 右(长按)
	KEY_DOWN_L,		// 下(长按)
	KEY_LEFT_L,		// 左(长按)
	KEY_MOTOR_REV_L,	// 电机反转(长按)
	KEY_MOTOR_FWD_L	// 电机正转(长按)
};

// 显示模式
enum DisplayMode {
	FUNCTION_SELECT = 9,	// 功能选择
	DEGREE_SETTING,			// 角度设置
	DEGREE_SHOW,			// 角度显示
	PRE_DEGREE_SELECT,		// 预设角度选择
};
//时间申明
time_t now;
struct tm *timenow;
/* 出错处理宏供返回错误码的函数使用*/
#define handle_error_en(en, msg) do{errno=en; perror(msg); exit(EXIT_FAILURE);} while(0)
/* 出错处理宏*/
#define handle_error(msg) do{perror(msg); exit(EXIT_FAILURE);} while(0)
#define handle_err_log(x, args...) do{fprintf(stderr, x"\n", ##args); exit(EXIT_FAILURE);}while(0)
#define log(x, args...) do{printf("file:%s function:%s line:%d "x"\n", __FILE__, __FUNCTION__, __LINE__, args);}while(0)

// DELAY
#define KEY_SCAN_DELAY_US 20*1000	// 键盘扫描间隔
#define DIGITRON_DELAY_US 3*1000	// 数码管动态显示间隔
#define DATA_PARSE_DELAY_US 50*1000		// Socket Server数据解析间隔

#endif /* COMMON_H_ */
