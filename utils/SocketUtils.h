/**
 * File name: SocketUtils.h
 * Author: JoshuaWong
 * Email: 1034316376@qq.com
 * Version: 1.0
 * Created on: Jun 28, 2017 10:39:05 AM
 * Description: 
 */

#ifndef UTILS_SOCKETUTILS_H_
#define UTILS_SOCKETUTILS_H_

#include "../common.h"

namespace SocketUtils {
inline int GetDownstreamDataLen(uint8_t cmd);
inline int GetUpstreamDataLen(uint8_t cmd);
}

namespace SocketUtils {
int GetDownstreamDataLen(uint8_t cmd) {
	int len;
	switch (cmd) {
	case CMD_HEARTBEAT:
		len = 0;
		break;
	case CMD_ONLINE:
		len = 11;
		break;
	case CMD_CORRECT:
	case CMD_STOP:
	case CMD_RUN_PARAM_QUERY:
	case CMD_CORRECT_QUERY:
	case CMD_TEN_POINT_QUERY:
		len = 1;
		break;
	case CMD_ROTATE_TO_DEGREE:
	case CMD_ROTATE_TO_POINT:
	case CMD_ROTATE_FWD:
	case CMD_ROTATE_REV:
		len = 3;
		break;
	case CMD_TEN_POINT_SET:
		len = 21;
		break;
	default:
		len = -1;
		break;
	}
	return len;
}

int GetUpstreamDataLen(uint8_t cmd) {
	int len;
	switch (cmd) {
	case CMD_BACK_HEARTBEAT:
		len = 0;
		break;
	case CMD_BACK_ONLINE:
		len = 11;
		break;
	case CMD_BACK_RUN_PARAM:
		len = 15;
		break;
	case CMD_BACK_TEN_POINT:
		len = 23;
		break;
	case CMD_BACK_CORRECT:
		len = 2;
		break;
	case CMD_BACK_CORRECT_QUERY:
		len = 3;
		break;
	default:
		len = -1;
		break;
	}
	return len;
}
}

#endif /* UTILS_SOCKETUTILS_H_ */
