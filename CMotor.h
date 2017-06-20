/**
 * File name: CMotor.h
 * Author: JoshuaWong
 * Email: 1034316376@qq.com
 * Version: 1.0
 * Created on: 2017年6月20日 上午10:52:05
 * Description: 
 */

#ifndef CMOTOR_H_
#define CMOTOR_H_

#include "common.h"

class CMotor {
public:
	CMotor();
	~CMotor();
	void Run();

private:
	int mCurrDegree;
	int mAimDegree;

};

inline CMotor::CMotor() :
		mCurrDegree(1314), mAimDegree(1314) {
	printf("CMotor object created!\n");
}

inline CMotor::~CMotor() {
	printf("CMotor object destroyed!\n");
}

#endif /* CMOTOR_H_ */
