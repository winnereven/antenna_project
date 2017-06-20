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
#include <iostream>
#include <unistd.h>
#include <stdint.h>
#include <errno.h>
#include <string>

#define uchar unsigned char

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

using namespace std;

/* 出错处理宏供返回错误码的函数使用*/
#define handle_error_en(en, msg) do{errno=en; perror(msg); exit(EXIT_FAILURE);} while(0)
/* 出错处理宏*/
#define handle_error(msg) do{perror(msg); exit(EXIT_FAILURE);} while(0)
#define handle_err_log(x, args...) do{fprintf(stderr, x"\n", ##args); exit(EXIT_FAILURE);}while(0)
#define log(x, args...) do{printf("file:%s function:%s line:%d "x"\n", __FILE__, __FUNCTION__, __LINE__, args);}while(0)

#endif /* COMMON_H_ */
