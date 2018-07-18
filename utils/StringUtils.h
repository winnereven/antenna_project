/**
 * File name: StringUtils.h
 * Author: JoshuaWong
 * Email: 1034316376@qq.com
 * Version: 1.0
 * Created on: 2017年6月21日 下午9:23:28
 * Description: 
 */

#ifndef UTILS_STRINGUTILS_H_
#define UTILS_STRINGUTILS_H_

#include "../common.h"

using namespace std;

namespace StringUtils {
// 分离字符串
inline vector<string> Split(string str, string pattern);
// int型转string型
inline string Int2Str(const int int_temp);
// string型转int型
inline int Str2Int(const string string_temp);
}

namespace StringUtils {
vector<string> Split(string str, string pattern) {
	string::size_type pos;
	vector<string> result;
	str += pattern; //扩展字符串以方便操作
	int size = str.size();

	for (int i = 0; i < size; i++) {
		pos = str.find(pattern, i);
		if ((int)pos < size) {
			string s = str.substr(i, pos - i);
			result.push_back(s);
			i = pos + pattern.size() - 1;
		}
	}
	return result;
}
string Int2Str(int int_temp) {
	ostringstream stream;
	stream << int_temp;
	return stream.str();   //此处也可以用stream>>string_temp
}
int Str2Int(string string_temp) {
	int int_temp;
	istringstream stream(string_temp);
	stream >> int_temp;
	return int_temp;
}
}

#endif /* UTILS_STRINGUTILS_H_ */
