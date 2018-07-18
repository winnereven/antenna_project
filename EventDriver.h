/**
 * File name: EventDriver.h
 * Author: JoshuaWong
 * Email: 1034316376@qq.com
 * Version: 1.0
 * Created on: 2017��6��15�� ����11:55:11
 * Description: 
 * Revised record:	time		name	content				line
 * 					20180312	even	����ѡ���а�����		183
 */

#ifndef EVENTDRIVER_H_
#define EVENTDRIVER_H_

#include "common.h"
#include "utils/DigUtils.h"
#include "IOnDegreeChangedListener.h"
#include "key/CKeyScan.h"
#include "key/IOnKeyClickListener.h"
#include "socket/IOnGetDownCmdListener.h"
#include "socket/SocketServer.h"
#include "socket/SocketMsg.h"
#include "CDigitron.h"
#include "CMotor.h"

class EventDriver: IOnDegreeChangedListener, IOnKeyClickListener, IOnGetDownCmdListener {
public:
	EventDriver(CKeyScan* keyScan, CDigitronSPI* digitron, SocketServer* server, CMotor* motor);
	~EventDriver();

	// @Override	�����߽Ƕȷ����仯ʱ����
	virtual void onDegreeChanged(int degree);
	// @Override
	virtual void onDegreeChangeFinished();
	// @Override	���а�������ʱ����
	virtual void onKeyClick(int key);
	// @Override	����/Զ��״̬�ı�ʱ����
	virtual void onCtlModeChanged(bool isLocal);
	// @Override	��SocketServer���յ����в���ָ��ʱ����
	virtual Msg* onGetDownCmd(Msg* down);

private:
	CKeyScan* mKeyScan;
	CDigitronSPI* mDigitron;
	SocketServer* mSocketServer;
	CMotor* mMotor;
	DisplayMode mMode;
	int mCurrFuncIndex;
	int mCurrFunction;
	int mCurrPreDegree;

	// ����ѡ��ģʽ��������
	string __FuncSelProcessor(int key);
	// �Ƕ�����ģʽ��������
	string __FuncOneProcessor(int key);
	// Ԥ��Ƕ�ѡ�񰴼�����
	string __FuncTwoProcessor(int key);
	// �Ƕ���ʾģʽ��������
	string __FuncThreeProcessor(int key);
	// �Ƕ�У׼��������
	string __FuncFourProcessor(int key);
	// Ԥ��Ƕ����ð�������
	string __FuncFiveProcessor(int key);
	// ��ԽǶ����ð�������
	string __FuncSixProcessor(int key);

	Msg* __GetRunParamsReplyMsg(uint8_t cmd);

};

inline EventDriver::EventDriver(CKeyScan* keyScan, CDigitronSPI* digitron, SocketServer* server,
		CMotor* motor) :
		mKeyScan(keyScan), mDigitron(digitron), mSocketServer(server), mMotor(motor),
		mMode(FUNCTION_SELECT), mCurrFuncIndex(0), mCurrFunction(-1), mCurrPreDegree(0) {
	printf("EventDriver Object created!\n");
	mDigitron->SetText(DigUtils::GetDisplayString(mMode, mCurrFuncIndex));
	mMotor->SetOnDegreeChangedListener(this);
	mKeyScan->SetOnClickListener(this);//onCtlModeChanged  &  onKeyClick
	mSocketServer->SetGetDownMsgListener(this);
	mMotor->SetGetDownMsgListener(this);
}

inline EventDriver::~EventDriver() {
	printf("EventDriver Object destroyed!\n");
}

inline void EventDriver::onCtlModeChanged(bool isLocal) {
	log("isLocalCtlMode:%d", isLocal);
	string temp_lory;
	mCurrFunction = 0;
	if(isLocal)
		temp_lory="L-01";
	else
		temp_lory="y-01";
	mDigitron->SetText(temp_lory);
	mDigitron->SetPointEnable(false);
	mDigitron->SetCursorPos(-1);

	sleep(1);
	mMode = DEGREE_SHOW;
	mDigitron->SetText(DigUtils::GetDisplayString(mMode, mMotor->GetCurrDegree()));
	mDigitron->SetPointEnable(true);
	mDigitron->SetCursorPos(-1);

	mSocketServer->SetEnable(!isLocal);
}

inline void EventDriver::onKeyClick(int key) {
	if (mMotor->IsRunning() && key != 9 && key != 10) // �����ת�����а�����Ч
		return;
	string disStr;

	if (key == KEY_MODE) {
		mCurrFunction = -1;		// ���ع���ѡ��
		mMode = FUNCTION_SELECT;
		mCurrFuncIndex = 0;
		disStr = DigUtils::GetDisplayString(mMode, mCurrFuncIndex);
		mDigitron->SetCursorPos(-1);
		mDigitron->SetPointEnable(false);
	} else
		switch (mCurrFunction) {
		case -1:	// ����ѡ��
			disStr = __FuncSelProcessor(key);
			break;
		case 0:		// ����1 F001
			disStr = __FuncOneProcessor(key);
			break;
		case 1:		// ����2 F002
			disStr = __FuncTwoProcessor(key);
			break;
		case 2:		// ����3 F003
			disStr = __FuncThreeProcessor(key);
			break;
		case 3:		// ����4 F004
			disStr = __FuncFourProcessor(key);
			break;
		case 4:		// ����5 F005
			disStr = __FuncFiveProcessor(key);
			break;
		case 5:		// ����6 F006
			disStr = __FuncSixProcessor(key);
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
		mCurrFuncIndex = (mCurrFuncIndex + 1) % 6;
		str = DigUtils::GetDisplayString(mMode, mCurrFuncIndex);
		break;
	case KEY_DOWN:
		mCurrFuncIndex = (mCurrFuncIndex + 5) % 6;
		str = DigUtils::GetDisplayString(mMode, mCurrFuncIndex);
		break;
	case KEY_OK:
		mCurrFunction = mCurrFuncIndex;
		switch (mCurrFuncIndex) {
		case 0:		// ����1ʱ����ת���Ƕ����ý���
			mMode = DEGREE_SETTING;
			str = DigUtils::GetDisplayString(mMode, mMotor->GetCurrDegree());
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
			str = DigUtils::GetDisplayString(mMode, mMotor->GetCurrDegree());
			mDigitron->SetPointEnable(true);
			break;
		case 3:
			mMode = DEGREE_SHOW;
			str = DigUtils::GetDisplayString(mMode, mMotor->GetCurrDegree());
			mDigitron->SetPointEnable(true);
			break;
		case 4:
			mMode = PRE_DEGREE_SELECT;
			mCurrPreDegree = 0;
			str = DigUtils::GetDisplayString(mMode, mCurrPreDegree);
			break;
		case 5:
			mMode = DEGREE_SETTING;
			str = DigUtils::GetDisplayString(mMode, 0);
			mDigitron->SetCursorPos(0);
			mDigitron->SetPointEnable(true);
			break;
		case 6:
			break;
		}
	case KEY_BACK:
		if(mMode == FUNCTION_SELECT)
		{
			mMode = DEGREE_SHOW;
			str = DigUtils::GetDisplayString(mMode, mMotor->GetCurrDegree());
			mDigitron->SetPointEnable(true);
			mCurrFunction = 5;
		}

		break;
	}
	return str;
}

inline string EventDriver::__FuncOneProcessor(int key) {
	string str = "";
	int newDegree;
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
		if (mMode == DEGREE_SHOW) {
			mMode = DEGREE_SETTING;
			mDigitron->SetCursorPos(0);
		} else {
			mCurrFunction = -1;		// ���ع���ѡ��
			mMode = FUNCTION_SELECT;
			str = DigUtils::GetDisplayString(mMode, mCurrFuncIndex);
			mDigitron->SetCursorPos(-1);
			mDigitron->SetPointEnable(false);
		}
		break;
//TODO ��ť�������
	case KEY_OK:
		mMode = DEGREE_SHOW;
		mDigitron->SetCursorPos(-1);
		newDegree = mDigitron->GetDegree();
		mMotor->RunToDegree(newDegree);
		break;
	}
	return str;
}

inline string EventDriver::__FuncTwoProcessor(int key) {
	string str = "";
	int degree = mMotor->GetCurrDegree();
	switch (key) {
	case KEY_UP:
		mCurrPreDegree = (mCurrPreDegree + 1) % 10;
		str = DigUtils::GetDisplayString(mMode, mCurrPreDegree);
		break;
	case KEY_DOWN:
		mCurrPreDegree = (mCurrPreDegree + 9) % 10;
		str = DigUtils::GetDisplayString(mMode, mCurrPreDegree);
		break;
	case KEY_BACK:
		if (mMode == DEGREE_SHOW) {
			mMode = PRE_DEGREE_SELECT;
			mDigitron->SetPointEnable(false);
			str = DigUtils::GetDisplayString(mMode, mCurrPreDegree);
		} else {
			mCurrFunction = -1;
			mMode = FUNCTION_SELECT;
			str = DigUtils::GetDisplayString(mMode, mCurrFuncIndex);
		}
		break;
	case KEY_OK:
		mMode = DEGREE_SHOW;
		degree = mMotor->GetPreDegree(mCurrPreDegree);
		mDigitron->SetPointEnable(true);
		mMotor->RunToDegree(degree);
		break;
	default:
		break;
	}
	return str;
}

inline string EventDriver::__FuncThreeProcessor(int key) {
	string str = "";

	switch (key) {
	case KEY_BACK:
		mCurrFunction = -1;
		mMode = FUNCTION_SELECT;
		str = DigUtils::GetDisplayString(mMode, mCurrFuncIndex);
		mDigitron->SetPointEnable(false);
		break;
	case KEY_OK:
		break;
//TODO ��ť�㶯ģʽ
	case KEY_MOTOR_FWD:
		if (mMotor->IsRunning())
			mMotor->StopRun();
		else
			mMotor->StepFWD();
		break;
	case KEY_MOTOR_REV:
		if (mMotor->IsRunning())
			mMotor->StopRun();
		else
			mMotor->StepREV();
		break;
	case KEY_MOTOR_FWD_L:
		mMotor->StartRunFWD();
		break;
	case KEY_MOTOR_REV_L:
		mMotor->StartRunREV();
		break;
	default:
		break;
	}

	return str;
}
//����Ԥ��Ķ���ָ��Ƕ�
inline string EventDriver::__FuncFiveProcessor(int key) {
	string str;
	int degree = mMotor->GetCurrDegree();
	switch (key) {
	case KEY_UP:
		if (mMode == DEGREE_SETTING) {
			mDigitron->CursorDataIncrease();
		} else {	// PRE_DEGREE_SELECT
			mCurrPreDegree = (mCurrPreDegree + 1) % 10;
			str = DigUtils::GetDisplayString(mMode, mCurrPreDegree);
		}
		break;
	case KEY_DOWN:
		if (mMode == DEGREE_SETTING) {
			mDigitron->CursorDataDecrease();
		} else {	// PRE_DEGREE_SELECT
			mCurrPreDegree = (mCurrPreDegree + 9) % 10;
			str = DigUtils::GetDisplayString(mMode, mCurrPreDegree);
		}
		break;
	case KEY_LEFT:
		if (mMode == DEGREE_SETTING)
			mDigitron->CursorLeftShift();
		break;
	case KEY_RIGTH:
		if (mMode == DEGREE_SETTING)
			mDigitron->CursorRightShift();
		break;
	case KEY_BACK:
		if (mMode == DEGREE_SHOW || mMode == DEGREE_SETTING) {
			mMode = PRE_DEGREE_SELECT;
			mDigitron->SetPointEnable(false);
			mDigitron->SetCursorPos(-1);
			str = DigUtils::GetDisplayString(mMode, mCurrPreDegree);
		} else {
			mCurrFunction = -1;
			mMode = FUNCTION_SELECT;
			str = DigUtils::GetDisplayString(mMode, mCurrFuncIndex);
		}
		break;
	case KEY_OK:
		if (mMode == DEGREE_SETTING) {
			degree = mDigitron->GetDegree();
			mMotor->SetPreDegree(degree, mCurrPreDegree);
			mMode = PRE_DEGREE_SELECT;
			str = DigUtils::GetDisplayString(mMode, mCurrPreDegree);
			mDigitron->SetPointEnable(false);
			mDigitron->SetCursorPos(-1);
		} else {	// PRE_DEGREE_SELECT
			mMode = DEGREE_SETTING;
			degree = mMotor->GetPreDegree(mCurrPreDegree);
			str = DigUtils::GetDisplayString(mMode, degree);
			mDigitron->SetPointEnable(true);
			mDigitron->SetCursorPos(0);
		}
		break;
	default:
		break;
	}
	return str;
}
//У׼��ǰλ��Ϊ0��
inline string EventDriver::__FuncFourProcessor(int key) {
	string str;
	switch (key) {
	case KEY_BACK:
		mCurrFunction = -1;
		mMode = FUNCTION_SELECT;
		mDigitron->SetPointEnable(false);
		str = DigUtils::GetDisplayString(mMode, mCurrFuncIndex);
		break;
	case KEY_CORRECT:
		mMotor->Correct();
		str = DigUtils::GetDisplayString(mMode, mMotor->GetCurrDegree());
		break;
	default:
		break;
	}
	return str;
}
//��ת��ԽǶ�
inline string EventDriver::__FuncSixProcessor(int key) {
	string str = "";
	int newDegree;
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
		if (mMode == DEGREE_SHOW) {
			mMode = DEGREE_SETTING;
			mDigitron->SetCursorPos(0);
		} else {
			mCurrFunction = -1;		// ���ع���ѡ��
			mMode = FUNCTION_SELECT;
			str = DigUtils::GetDisplayString(mMode, mCurrFuncIndex);
			mDigitron->SetCursorPos(-1);
			mDigitron->SetPointEnable(false);
		}
		break;
//TODO ��ť�������
	case KEY_OK:
		mMode = DEGREE_SHOW;
		mDigitron->SetCursorPos(-1);
		newDegree = mDigitron->GetDegree()/10;
		str = DigUtils::GetDisplayString(mMode, mMotor->GetCurrDegree());
		mMotor->RunToDegree(newDegree+mMotor->GetCurrDegree());
		break;
	}
	return str;

}

inline Msg* EventDriver::__GetRunParamsReplyMsg(uint8_t cmd) {
	Msg *backMsg = NULL;
	uint8_t data[SOCKET_MSG_DATA_SIZE];
	data[0] = 0x01;	// ��ַ��
	data[1] = cmd;	// ��Ӧ����������
	data[2] = mMotor->IsRunning() ? 0x01 : 0x00;	// ״̬��Ϣ
	data[3] = mKeyScan->GetCtlMode() ? 0x01 : 0x02;	// ����ģʽ
	uchar *iw;
	iw = mMotor->back_WorkStation();
	for (int i = 0; i < 9; i++) {
		data[i+4] = *(iw+i);
	}
	data[13] = mMotor->GetCurrDegree() >> 8;
	data[14] = mMotor->GetCurrDegree() & 0xff;

	backMsg = new Msg(0,CMD_BACK_RUN_PARAM, data, 15, false);
	return backMsg;
}

inline void EventDriver::onDegreeChanged(int degree) {
	log("onDegreeChanged:%d", degree);
	mMotor->SetCrutDegre(degree);
	if (mMode == DEGREE_SHOW)
		mDigitron->SetText(DigUtils::GetDisplayString(DEGREE_SHOW, degree));

	mSocketServer->SendMsg(__GetRunParamsReplyMsg(CMD_RUN_PARAM_QUERY));
}

inline void EventDriver::onDegreeChangeFinished() {

}
// TODO ���ս���
inline Msg* EventDriver::onGetDownCmd(Msg* down) {
	Msg *backMsg = NULL;
	uint8_t data[SOCKET_MSG_DATA_SIZE];
	int degree;
	switch (down->cmd) {
	case CMD_HEARTBEAT:		// ����ָ��
		backMsg = __GetRunParamsReplyMsg(CMD_HEARTBEAT);
		break;
	case CMD_ONLINE:		// ����ָ��
		data[0] = 0x02;
		for (int i = 1; i < 11; i++)
			data[i] = 0x00;
		backMsg = new Msg(0,CMD_BACK_ONLINE, data, 11, false);
		mMotor->back_WorkStation();
		break;
	case CMD_ROTATE_TO_DEGREE:	// ��λ����ָ��
		if (mMotor->IsRunning() || mKeyScan->GetCtlMode())
			break;
		degree = down->data[1] * 256 + down->data[2];
		log("degree:%d", degree);
		mMotor->RunToDegree(degree);

		data[0] = 0x01;	// ��ַ��
		data[1] = CMD_ROTATE_TO_DEGREE;	// ��Ӧ����������
		data[2] = mMotor->IsRunning() ? 0x01 : 0x00;	// ״̬��Ϣ
		data[3] = mKeyScan->GetCtlMode() ? 0x01 : 0x02;	// ����ģʽ
		uchar *iw;
		iw = mMotor->back_WorkStation();
		for (int i = 0; i < 9; i++) {
			data[i+4] = *(iw+i);
		}
		data[13] = mMotor->GetCurrDegree() >> 8;
		data[14] = mMotor->GetCurrDegree() & 0xff;

		backMsg = new Msg(0,CMD_BACK_RUN_PARAM, data, 15, false);
		break;
//TODO ��λ���������
	case CMD_ROTATE_TO_POINT:	// �������ָ��
		if (mMotor->IsRunning() || mKeyScan->GetCtlMode())
			break;
		degree = down->data[1] * 256 + down->data[2];
		log("degree:%d", degree);
		mMotor->RunToDegree(degree);
		break;
	case CMD_CORRECT:	// �Ƕ�У׼ָ��
		if (mMotor->IsRunning() || mKeyScan->GetCtlMode())
			break;
		mMotor->Correct();
		mDigitron->SetText(DigUtils::GetDisplayString(mMode, mMotor->GetCurrDegree()));
		break;
	case CMD_ROTATE_FWD:	// ˳ʱ��㶯����
		if (mMotor->IsRunning() || mKeyScan->GetCtlMode())
			break;
		degree = down->data[1] * 256 + down->data[2];
		log("degree:%d", degree);
		mMotor->StepFWD(degree);
		break;
	case CMD_ROTATE_REV:	// ��ʱ��㶯����
		if (mMotor->IsRunning() || mKeyScan->GetCtlMode())
			break;
		degree = down->data[1] * 256 + down->data[2];
		log("degree:%d", degree);
		mMotor->StepREV(degree);
		break;
	case CMD_STOP:	// ����ͣ��ָ��
		if (mKeyScan->GetCtlMode())
			break;
		mMotor->StopRun();
		backMsg = __GetRunParamsReplyMsg(CMD_STOP);
		break;
	case CMD_RUN_PARAM_QUERY:	// ����������ѯ
		backMsg = __GetRunParamsReplyMsg(CMD_RUN_PARAM_QUERY);
		break;
	case CMD_CORRECT_QUERY:		// У׼�ǶȲ�ѯ
		if (down->data[0] == ADDRESS) {
			data[0] = ADDRESS;
			data[1] = mMotor->GetOffsetDegree() >> 8;
			data[2] = mMotor->GetOffsetDegree() & 0xff;
			backMsg = new Msg(0,CMD_BACK_CORRECT_QUERY, data, 3, false);
		}
		break;
	case CMD_TEN_POINT_QUERY:	// 10������ǶȲ�ѯ
		if (down->data[0] == ADDRESS) {
			data[0] = ADDRESS;
			data[1] = CMD_TEN_POINT_QUERY;
			data[2] = 0x00;		// Ԥ��λ
			for (int i = 0; i < 10; i++) {
				data[2 * i + 3] = mMotor->GetPreDegree(i) >> 8;
				data[2 * i + 4] = mMotor->GetPreDegree(i) & 0xff;
			}
			backMsg = new Msg(0,CMD_BACK_TEN_POINT, data, 23, false);
		}
		break;
	case CMD_TEN_POINT_SET:		// 10������Ƕ�����
		if (mKeyScan->GetCtlMode())
			break;
		for (int i = 0; i < 10; i++) {
			degree = down->data[2 * i + 1] * 256 + down->data[2 * i + 2];
			mMotor->SetPreDegree(degree, i);
		}
		break;
	case CMD_RUN_DEGREE:	//��ת��ԽǶ�
		degree = down->data[1] * 256 + down->data[2];
		degree /= 10;
		mMotor->RunToDegree(degree+mMotor->GetCurrDegree());
		backMsg = __GetRunParamsReplyMsg(CMD_RUN_DEGREE);
		break;
	case CMD_ADU_REBOOT:	//ADU reboot
//		exit(EXIT_SUCCESS);
		break;
		//TODO �޸�FPGA�ش�ָ���
	case CMD_FPGA_BACK:
		int back_degree ;
//		printf("msg data is ");
//		for(int i=0;i<down->dataLen;i++)
//		{
//			printf("%x ",down->data[i]);
//		}
//		printf("\n");
		back_degree = down->data[9]*256+down->data[10];
		back_degree /= 10;
		printf("back_degree is %d \n",back_degree);
		if(back_degree!=mMotor->GetCurrDegree())
			mMotor->SetCurrDegree(back_degree);
		else if(mMotor->IsRunning())
			mMotor->SetStop();
		else
			;
		break;

	default:
		break;
	}
	return backMsg;
}

#endif /* EVENTDRIVER_H_ */
