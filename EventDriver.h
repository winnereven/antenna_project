/**
 * File name: EventDriver.h
 * Author: JoshuaWong
 * Email: 1034316376@qq.com
 * Version: 1.0
 * Created on: 2017年6月15日 上午11:55:11
 * Description: 
 */

#ifndef EVENTDRIVER_H_
#define EVENTDRIVER_H_

#include "common.h"
#include "CDigitronSPI.h"
#include "utils/DigUtils.h"

class EventDriver {
public:
	EventDriver(CDigitronSPI* digitron, CMotor* motor);
	~EventDriver();
	void KeyClick(int key);

private:
	CDigitronSPI * mDigitron;
	CMotor* mMotor;
	DisplayMode mMode;
	int mCurrFunction;
	int mCurrPreDegree;

	// 功能选择模式按键处理
	string __FuncSelProcessor(int key);
	// 角度设置模式按键处理
	string __DegreeSetProcessor(int key);
	// 角度显示模式按键处理
	string __DegreeShowProcessor(int key);
	// 预设角度选择按键处理
	string __PreDegreeSelProcessor(int key);
};

inline EventDriver::EventDriver(CDigitronSPI* digitron, CMotor* motor) :
		mDigitron(digitron), mMotor(motor), mMode(FUNCTION_SELECT), mCurrFunction(0), mCurrPreDegree(
				0) {
	printf("EventDriver Object created!\n");
	mDigitron->SetText(DigUtils::GetDisplayString(mMode, mCurrFunction));
}

inline EventDriver::~EventDriver() {
	printf("EventDriver Object destroyed!\n");
}

inline void EventDriver::KeyClick(int key) {
	string disStr;
	switch (mMode) {
	case FUNCTION_SELECT:
		disStr = __FuncSelProcessor(key);
		break;
	case DEGREE_SETTING:
		disStr = __DegreeSetProcessor(key);
		break;
	case PRE_DEGREE_SELECT:
		disStr = __PreDegreeSelProcessor(key);
		break;
	case DEGREE_SHOW:
		disStr = __DegreeShowProcessor(key);
		break;
	default:
		break;
	}
	log("%s", disStr.c_str());
	if (disStr != "")
		mDigitron->SetText(disStr);
}

inline string EventDriver::__FuncSelProcessor(int key) {
	string str = "";
	switch (key) {
	case KEY_UP:
		mCurrFunction = (mCurrFunction + 1) % 7;
		str = DigUtils::GetDisplayString(mMode, mCurrFunction);
		break;
	case KEY_DOWN:
		mCurrFunction = (mCurrFunction + 6) % 7;
		str = DigUtils::GetDisplayString(mMode, mCurrFunction);
		break;
	case KEY_OK:
		switch (mCurrFunction) {
		case 0: // 功能1时，跳转到角度设置界面
			mMode = DEGREE_SETTING;
			str = DigUtils::GetDisplayString(mMode, mDegree);
			mDigitron->SetCursorPos(0);
			mDigitron->SetPointEnable(true);
			break;
		case 1:
			mMode = PRE_DEGREE_SELECT;
			mCurrPreDegree = 0;
			str = DigUtils::GetDisplayString(mMode, mCurrPreDegree);
			break;
		case 2:
			mMode = DEGREE_SHOW;
			str = DigUtils::GetDisplayString(mMode, mDegree);
			mDigitron->SetPointEnable(true);
			break;
		case 3:
			break;
		case 4:
			break;
		case 5:
			break;
		case 6:
			break;
		}
		break;
	}
	return str;
}

inline string EventDriver::__DegreeSetProcessor(int key) {
	string str = "";
	switch (key) {
	case KEY_LEFT:
		mDigitron->CursorLeftShift();
		break;
	case KEY_RIGTH:
		mDigitron->CursorRightShift();
		break;
	case KEY_UP:
		mDigitron->CursorDataIncrease();
		break;
	case KEY_DOWN:
		mDigitron->CursorDataDecrease();
		break;
	case KEY_BACK:
		mMode = FUNCTION_SELECT;
		str = DigUtils::GetDisplayString(mMode, mCurrFunction);
		mDigitron->SetCursorPos(-1);
		mDigitron->SetPointEnable(false);
		break;
	case KEY_OK:
		mDigitron->SetCursorPos(-1);
		mDigitron->GetDegree(&mDegree);
		log("%d", mDegree);
		break;
	}
	return str;
}

inline string EventDriver::__DegreeShowProcessor(int key) {
	string str = "";

	switch (key) {
	case KEY_BACK:
		mMode = FUNCTION_SELECT;
		str = DigUtils::GetDisplayString(mMode, mCurrFunction);
		mDigitron->SetPointEnable(false);
		break;
	case KEY_OK:
		mDigitron->SetCursorPos(-1);
		mDigitron->GetDegree(&mDegree);
		log("%d", mDegree);
		break;
	case KEY_MOTOR_FWD:

		break;
	case KEY_MOTOR_REV:

		break;
	default:
		break;
	}

	return str;
}

inline string EventDriver::__PreDegreeSelProcessor(int key) {
	string str = "";
	switch (key) {
	case KEY_UP:
		mCurrPreDegree = (mCurrPreDegree + 1) % 10;
		str = DigUtils::GetDisplayString(mMode, mCurrPreDegree);
		break;
	case KEY_DOWN:
		mCurrPreDegree = (mCurrPreDegree + 9) % 10;
		str = DigUtils::GetDisplayString(mMode, mCurrPreDegree);
		break;
	case KEY_OK:
		break;
	case KEY_BACK:
		mMode = FUNCTION_SELECT;
		str = DigUtils::GetDisplayString(mMode, mCurrFunction);
		break;
	default:
		break;
	}
	return str;
}

#endif /* EVENTDRIVER_H_ */
