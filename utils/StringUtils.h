/**
 * File name: StringUtils.h
 * Author: JoshuaWong
 * Email: 1034316376@qq.com
 * Version: 1.0
 * Created on: 2017��6��21�� ����9:23:28
 * Description: 
 */

#ifndef UTILS_STRINGUTILS_H_
#define UTILS_STRINGUTILS_H_

#include "../common.h"

using namespace std;

namespace StringUtils {
// �����ַ���
inline vector<string> Split(string str, string pattern);
// int��תstring��
inline string Int2Str(const int int_temp);
// string��תint��
inline int Str2Int(const string string_temp);
}

namespace StringUtils {
vector<string> Split(string str, string pattern) {
	string::size_type pos;
	vector<string> result;
	str += pattern; //��չ�ַ����Է������
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
	return stream.str();   //�˴�Ҳ������stream>>string_temp
}
int Str2Int(string string_temp) {
	int int_temp;
	istringstream stream(string_temp);
	stream >> int_temp;
	return int_temp;
}
}

#endif /* UTILS_STRINGUTILS_H_ */
