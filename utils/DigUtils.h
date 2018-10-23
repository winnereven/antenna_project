/**
 * File name: DigUtils.h
 * Author: JoshuaWong
 * Email: 1034316376@qq.com
 * Version: 1.0
 * Created on: 2017年5月30日 下午8:40:26
 * Description: 
 */

#ifndef UTILS_DIGUTILS_H_
#define UTILS_DIGUTILS_H_

#include "../common.h"

using namespace std;

//不带小数点的段码(共阳极)
const uint8_t Dig_Figure_sun[] = { 0xc0 /* 0 */, 0xf9 /* 1 */, 0xa4 /* 2 */, 0xb0 /* 3 */,
		0x99 /* 4 */, 0x92 /* 5 */, 0x82 /* 6 */, 0xf8 /* 7 */, 0x80 /* 8 */, 0x90 /* 9 */,
		0xFF /* 全暗 */, 0xa7 /* C */, 0xbf /* - */, 0xa0 /* a */, 0xc7 /* L */, 0x8e /* F */,
		0x91 /* y */, 0x8c /* p */, 0xA3 /* o */, 0xA1 /* d */, 0xF1 /* j */
};

//不带小数点的段码(共阴极)
const uint8_t Dig_Figure[] = { 0x3f /* 0 */, 0x06 /* 1 */, 0x5b /* 2 */, 0x4f /* 3 */, 0x66 /* 4 */,
		0x6d /* 5 */, 0x7d /* 6 */, 0x07 /* 7 */, 0x7f /* 8 */, 0x6f /* 9 */, 0x00 /* 全暗 */,
		0x39 /* C */, 0x40 /* - */, 0x5F /* a */, 0x38 /* L */, 0x71 /* F */, 0x6e /* y */, 0x73 /* p */,
		0x5C /* o */, 0x5E /* d */, 0x0E /* j */
};

//带小数点的段码(共阴极)
const uint8_t Dig_Figure_Point[] = { 0xbf /* 0 */, 0x86 /* 1 */, 0xdb /* 2 */, 0xcf /* 3 */,
		0xe6 /* 4 */, 0xed /* 5 */, 0xfd /* 6 */, 0x87 /* 7 */, 0xff /* 8 */, 0xef /* 9 */,
		0x80 /* 全暗 */, 0xb9 /* C */, 0xc0 /* - */, 0xdF /* a */, 0xb8 /* L */, 0xf1 /* F */,
		0xee /* y */, 0xf3 /* p */
};

const char Dig_Char[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', ' ', 'C', '-', 'a', 'L',
		'F', 'y', 'p','o','d','j' };

const string DIS_STR_MODE[] = { "F001", "F002", "F003", "F004", "F005", "F006", "F007" };
const string DIS_STR_PRE_DEGREE[] = { "C001", "C002", "C003", "C004", "C005", "C006", "C007",
		"C008", "C009", "C010","--dj","--oL" };

namespace DigUtils {
/**
 * 获取字符编码
 */
inline uint8_t GetCodeOfChar(char c);
/**
 * 获取空白字符编码
 */
inline uint8_t GetBlankCode();
/**
 * 获取显示字符
 * mode:当前显示模式	extra:附加参数
 */
inline string GetDisplayString(DisplayMode mode, int extra);
}

namespace DigUtils {

uint8_t GetCodeOfChar(char c) {
	int len = sizeof(Dig_Char) / sizeof(char);
	for (int i = 0; i < len; i++) {
		if (c == Dig_Char[i])
			return Dig_Figure[i];
	}
	return Dig_Figure[10];
}

uint8_t GetIndexOfCode(uint8_t code) {
	int len = sizeof(Dig_Figure) / sizeof(char);
	for (int i = 0; i < len; i++) {
		if (code == Dig_Figure[i])
			return i;
	}
	return 10;
}

uint8_t GetBlankCode() {
	return Dig_Figure[10];
}

string GetDisplayString(DisplayMode mode, int extra) {
	string str = "";
	char s[4];
	switch (mode) {
	case FUNCTION_SELECT:
		extra %= 7;
		str = DIS_STR_MODE[extra];
		break;
	case DEGREE_SETTING:
		sprintf(s, "%04d", extra);
		str = s;
		break;
	case DEGREE_SHOW:
		if (extra == 0)
			sprintf(s, "  00");
		else
			sprintf(s, "%4d", extra);
		str = s;
		break;
	case PRE_DEGREE_SELECT:
		extra %= 12;
		str = DIS_STR_PRE_DEGREE[extra];
		break;
	default : break;
	}
	log("%s", str.c_str());
	return str;
}
}

#endif /* UTILS_DIGUTILS_H_ */
