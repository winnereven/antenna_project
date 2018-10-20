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
#include "IOnDegreeChangedListener.h"
#include "utils/StringUtils.h"
#include "uart/UartServer.h"
#include "CTehu485.h"

#define DELAY 40*1000
#define MOTOR_STEP 5
#define DEGREES_FILE_NAME "/opt/pre_degrees.dat"
#define CURDEGR_FILE_NAME "/opt/current_degrees.dat" //当前角度文件

#define DEV_FWD_LED "/dev/gpio-P2.20"
#define DEV_REV_LED "/dev/gpio-P2.21"
#define DEV_ERR_LED "/dev/gpio-ERR"
#define DEV_RUN_LED "/dev/gpio-RUN"
#define BMQ_SWITCH "/dev/gpio-P3.6"
#define MOTO_RDY "/dev/gpio-P3.26"

#define LED_SHINE_QUICK_COUNT	5	// *LED_DELAY_US
#define LED_SHINE_SLOW_COUNT	15
const bool LEDLIGHT = true;//高电平亮灯

const char* LED_CHANNEL[] = {DEV_RUN_LED,DEV_ERR_LED};
const uchar LEDNUM = 2;

class CMotor :public UartServer,public CTehu485{
public:
	CMotor();
	~CMotor();
	void Run();
	// 获取当前角度
	int GetCurrDegree();
	// 更新当前角度
	int SetCurrDegree(int currdegree);
	// 获取校准角度
	int GetOffsetDegree();

	// 电机当前是否在转动
	bool IsRunning();
	// 电机是否准备好
	bool IsReady();
	//电机停了
	void SetStop();
	// 电机正向步进一定角度
	void StepFWD();
	void StepFWD(int degree);
	// 电机反向步进一定角度
	void StepREV();
	void StepREV(int degree);

	// 电机转到目标角度
	void RunToDegree(int degree);

	// 电机正转
	void StartRunFWD();
	// 电机反转
	void StartRunREV();
	// 电机停转
	void StopRun();

	// 开关正反转按键灯
	void SetFwdLedOn(bool on);
	void SetRevLedOn(bool on);
	// 开关警告工作灯
	void SetErrLedOn(bool on);
	void SetRunLedOn(bool on);

	// 获得第N组预设角度
	void SetPreDegree(int degree, int index);
	int GetPreDegree(int index);

	// 校准
	void Correct();

	// 设置角度数据变化监听
	void SetOnDegreeChangedListener(IOnDegreeChangedListener* listener);
	//设置存储角度
	void SetCrutDegre(int degree);
	//设置LED显示状态
	void SetLedStatus(int iLed,uint8_t data);
	// 更新LED状态
	void LedRefresh();
	//设置编码器选项
	int SetBMQ(bool on);

//	int isCorrect;
private:
	int mCurrDegree;			// 电机当前角度
	int mIntentDegreeDiff;		// 电机目标角度差值	-1：持续转动	0~360：欲转相对角度
	int m_nOffset;
	bool m_bForward;
	int m_fdFwdLed, m_fdRevLed, m_fdErrLed, m_fdRunLed,m_fdBmpSw,m_fdIsready;
	vector<int> m_nDegrees;
	bool mIsruning;
	IOnDegreeChangedListener* mDegreeChangedListener;
	int m_fdLedWork[LEDNUM];	// LED1 LED2
	uchar m_nLedStatus[LEDNUM];	// 0或其他-灭，1-慢闪，2-快闪，3-长亮
	int m_nLedFlag[LEDNUM];		// LED闪烁计数

	// 加载预设10组本地角度数据
	void __LoadLocalData();
	void __SaveLocalData();
	void __LoadCurtDegre();
	void __SetGpioOn(int fd, bool on);

};

inline CMotor::CMotor() :
		mCurrDegree(0), mIntentDegreeDiff(0), m_bForward(true),mIsruning(0), mDegreeChangedListener(
		NULL) {
	printf("CMotor object created!\n");
	m_fdFwdLed = open(DEV_FWD_LED, O_RDWR);
	if (m_fdFwdLed < 0)
		handle_err_log("%s %s", DEV_FWD_LED, "open");
	m_fdRevLed = open(DEV_REV_LED, O_RDWR);
	if (m_fdRevLed < 0)
		handle_err_log("%s %s", DEV_REV_LED, "open");

	m_fdErrLed = open(DEV_ERR_LED, O_RDWR);
	if (m_fdErrLed < 0)
		handle_err_log("%s %s", DEV_ERR_LED, "open");
	m_fdRunLed = open(DEV_RUN_LED, O_RDWR);
	if (m_fdRunLed < 0)
		handle_err_log("%s %s", DEV_RUN_LED, "open");
	m_fdIsready = open(MOTO_RDY,O_RDWR);
	if (m_fdIsready < 0)
		handle_err_log("%s %s", MOTO_RDY, "open");

	for (int i = 0; i < LEDNUM; i++) {
		m_nLedStatus[i] = 0;
		m_fdLedWork[i] = open(LED_CHANNEL[i], O_RDWR);
	}
	m_fdBmpSw = open(BMQ_SWITCH, O_RDWR);
	if (m_fdRunLed < 0)
		handle_err_log("%s %s", BMQ_SWITCH, "open");

	int ret = ioctl(m_fdBmpSw, SET_GPIO_LOW);//默认为电机编码器
	if (ret < 0)
		handle_err_log("%s %s", BMQ_SWITCH, "ioctl");


	SetFwdLedOn(false);
	SetRevLedOn(false);

	SetErrLedOn(false);
	SetRunLedOn(true);

	__LoadLocalData();
	__LoadCurtDegre();

	this->SendtoFPGA(4,0,GetCurrDegree());
}

inline CMotor::~CMotor() {
	int ret = close(m_fdFwdLed);
	if (ret)
		handle_err_log("%s %s", DEV_FWD_LED, "close");
	ret = close(m_fdRevLed);
	if (ret)
		handle_err_log("%s %s", DEV_REV_LED, "close");
	ret = close(m_fdErrLed);
	if (ret)
		handle_err_log("%s %s", DEV_ERR_LED, "close");
	ret = close(m_fdRunLed);
	if (ret)
		handle_err_log("%s %s", DEV_RUN_LED, "close");
	printf("CMotor object destroyed!\n");
}

inline void CMotor::Run() {
	while (1) {
			this->Start();
//		if (mIntentDegreeDiff > 0) {
//			mCurrDegree += (m_bForward ? 3 : 3597);
//			mIntentDegreeDiff -= 3;
//			mCurrDegree %= 3600;
//			if (mDegreeChangedListener != NULL) {
//				mDegreeChangedListener->onDegreeChanged(mCurrDegree);
//				if (mIntentDegreeDiff <= 0) {
//					if (m_bForward)
//						SetFwdLedOn(false);
//					else
//						SetRevLedOn(false);
//					mDegreeChangedListener->onDegreeChangeFinished();
//				}
//			}
//		}
			time(&now);
			timenow = localtime(&now);
			if(mIsruning)
			{
				if(timenow->tm_sec%5 == 0)
					get_WorkStation(0);
			}else if(timenow->tm_min%3==0&&timenow->tm_sec==0){	// 3分钟采集一次
				get_WorkStation(0);
				get_WorkStation(1);
			}
			LedRefresh();
			usleep(DELAY);

	}
}

inline int CMotor::GetCurrDegree() {
	mCurrDegree %= 3600;
	return mCurrDegree;
}
inline int CMotor::SetCurrDegree(int currdegree) {
	mIsruning =1;
	mIntentDegreeDiff -=abs(currdegree-mCurrDegree);
	mCurrDegree = currdegree;
	mCurrDegree %= 3600;
	if (m_bForward)
		SetFwdLedOn(true);
	else
		SetRevLedOn(true);

	if (mDegreeChangedListener != NULL) {
		mDegreeChangedListener->onDegreeChanged(mCurrDegree);
//		if (mIntentDegreeDiff <= 0) {
//			if (m_bForward)
//				SetFwdLedOn(false);
//			else
//				SetRevLedOn(false);
//			mDegreeChangedListener->onDegreeChangeFinished();
//		}
//		else{
//			if (m_bForward)
//				SetFwdLedOn(true);
//			else
//				SetRevLedOn(true);
//		}
	}
	return mCurrDegree;
}

inline int CMotor::GetOffsetDegree() {
	return m_nOffset;
}

inline bool CMotor::IsRunning() {
	return mIntentDegreeDiff > 0||mIsruning;
}
inline bool CMotor::IsReady(){
	uint8_t buf;
	int ret = read(m_fdIsready, &buf, 1);
	if (ret < 0)
		handle_err_log("%s %s", DEV_STEP_L, "read");
	return buf != 0x00;
}
inline void CMotor::SetStop() {
	mIsruning=0;
	mIntentDegreeDiff=0;
	SetFwdLedOn(false);
	SetRevLedOn(false);
//	if (mDegreeChangedListener != NULL) {
//		mDegreeChangedListener->onDegreeChanged(mCurrDegree);
//	}
	SetCrutDegre(mCurrDegree);
}
inline void CMotor::StepFWD() {
	m_bForward = true;
	mIntentDegreeDiff = MOTOR_STEP;
	this->SendtoFPGA(1,mIntentDegreeDiff,this->GetCurrDegree());
}

inline void CMotor::StepFWD(int degree) {
	m_bForward = true;
	mIntentDegreeDiff = degree;
	this->SendtoFPGA(1,mIntentDegreeDiff,this->GetCurrDegree());
}

inline void CMotor::StepREV() {
	m_bForward = false;
	mIntentDegreeDiff = MOTOR_STEP;
	this->SendtoFPGA(0,mIntentDegreeDiff,this->GetCurrDegree());
}

inline void CMotor::StepREV(int degree) {
	m_bForward = false;
	mIntentDegreeDiff = degree;
	this->SendtoFPGA(0,mIntentDegreeDiff,this->GetCurrDegree());
}

inline void CMotor::RunToDegree(int degree) {
	if (mCurrDegree < degree && degree <= 1800 + mCurrDegree) {
		m_bForward = true;
		mIntentDegreeDiff = degree - mCurrDegree;
	} else if (mCurrDegree < degree && degree > 1800 + mCurrDegree) {
		m_bForward = false;
		mIntentDegreeDiff = mCurrDegree - degree + 3600;
	} else if (mCurrDegree > degree && mCurrDegree <= 1800 + degree) {
		m_bForward = false;
		mIntentDegreeDiff = mCurrDegree - degree;
	} else if (mCurrDegree > degree && mCurrDegree > 1800 + degree) {
		m_bForward = true;
		mIntentDegreeDiff = degree - mCurrDegree + 3600;
	}
	if (mIntentDegreeDiff > 0) {
		if (m_bForward)
		{
			SetFwdLedOn(true);
			this->SendtoFPGA(1,mIntentDegreeDiff,this->GetCurrDegree());
		}
		else
		{
			SetRevLedOn(true);
			this->SendtoFPGA(0,mIntentDegreeDiff,this->GetCurrDegree());
		}
	}
	log("%d", mIntentDegreeDiff);
}

inline void CMotor::StartRunFWD() {
	m_bForward = true;
	mIntentDegreeDiff = 35999;
	SetFwdLedOn(true);
	this->SendtoFPGA(3,mIntentDegreeDiff,this->GetCurrDegree());
}

inline void CMotor::StartRunREV() {
	m_bForward = false;
	mIntentDegreeDiff = 35999;
	SetRevLedOn(true);
	this->SendtoFPGA(2,mIntentDegreeDiff,this->GetCurrDegree());
}

inline void CMotor::StopRun() {
	SetCurrDegree(mCurrDegree);
	mIntentDegreeDiff = 0; // 不为0，可以触发onDegreeChangeFinished
	SetFwdLedOn(false);
	SetRevLedOn(false);

	this->SendtoFPGA(5,mIntentDegreeDiff,this->GetCurrDegree());
}

inline void CMotor::SetFwdLedOn(bool on) {
	int ret = ioctl(m_fdFwdLed, on ? SET_GPIO_HIGHT : SET_GPIO_LOW);
	if (ret < 0)
		handle_err_log("%s %s", DEV_FWD_LED, "ioctl");
}

inline void CMotor::SetRevLedOn(bool on) {
	int ret = ioctl(m_fdRevLed, on ? SET_GPIO_HIGHT : SET_GPIO_LOW);
	if (ret < 0)
		handle_err_log("%s %s", DEV_REV_LED, "ioctl");
}

inline void CMotor::SetErrLedOn(bool on) {
	int ret = ioctl(m_fdErrLed, on ? SET_GPIO_HIGHT : SET_GPIO_LOW);
	if (ret < 0)
		handle_err_log("%s %s", DEV_ERR_LED, "ioctl");
}
inline void CMotor::SetRunLedOn(bool on) {
	int ret = ioctl(m_fdRunLed, on ? SET_GPIO_HIGHT : SET_GPIO_LOW);
	if (ret < 0)
		handle_err_log("%s %s", DEV_RUN_LED, "ioctl");
}

inline void CMotor::SetPreDegree(int degree, int index) {
	m_nDegrees[index] = degree;
	__SaveLocalData();
}

inline int CMotor::GetPreDegree(int index) {
	return m_nDegrees[index];
}

inline void CMotor::Correct() {
	m_nOffset = (m_nOffset + mCurrDegree) % 3600;
	__SaveLocalData();
	mCurrDegree = 0;
//	isCorrect = 0;
	this->SendtoFPGA(4,0,0);
}

inline void CMotor::SetOnDegreeChangedListener(IOnDegreeChangedListener* listener) {
	this->mDegreeChangedListener = listener;
}

inline void CMotor::__LoadLocalData() {
	ifstream in(DEGREES_FILE_NAME);
	string line;

	if (in) {	// 有该文件
		getline(in, line);	// 取预设角度值行
		cout << line << endl;
		vector<string> strs = StringUtils::Split(line, " ");
		for (int i = 0; i < PRE_DEGREE_COUNT; i++) {
			m_nDegrees.push_back(StringUtils::Str2Int(strs[i]));
		}
		getline(in, line);	// 取校准偏移值行
		cout << line << endl;
		m_nOffset = StringUtils::Str2Int(line);
	} else
		cout << "no such file" << endl;		// 没有该文件
}
inline void CMotor::__LoadCurtDegre(){
	ifstream in(CURDEGR_FILE_NAME);
	string degree;

	if(in){
		getline(in,degree);
		cout << degree << endl;
		mCurrDegree = StringUtils::Str2Int(degree);
	}else
		cout<<"no such file"<<endl;
}
inline void CMotor::__SaveLocalData() {
	string str = StringUtils::Int2Str(m_nDegrees[0]);
	for (int i = 1; i < PRE_DEGREE_COUNT; i++) {		// 存储10组预设角度值
		str = str + " ";
		str = str + StringUtils::Int2Str(m_nDegrees[i]);
	}
	str = str + "\n" + StringUtils::Int2Str(m_nOffset);	// 存储校准偏移值

	const char *path = DEGREES_FILE_NAME;

	if (remove(path) == 0) {
		cout << "delete " << DEGREES_FILE_NAME << " successful!" << endl;
	} else {
		cout << "delete " << DEGREES_FILE_NAME << " failed!" << endl;
	}

	ofstream fout(DEGREES_FILE_NAME);
	fout << str << endl;
	fout.close();
}
inline void CMotor::SetCrutDegre(int degree){
	string str = StringUtils::Int2Str(degree);
	const char *path = CURDEGR_FILE_NAME;

	if(remove(path)==0){
		cout << "delete "<< CURDEGR_FILE_NAME << "successful! "<<endl;
	}else{
		cout << "delete "<< CURDEGR_FILE_NAME << "failed!" << endl;
	}

	ofstream fout(CURDEGR_FILE_NAME);
	fout << str << endl;
	fout.close();
}
inline void CMotor::SetLedStatus(int i,uint8_t idate) {
		m_nLedStatus[i] = idate;
}
inline void CMotor::LedRefresh() {
	for (int i = 0; i < LEDNUM; i++) {
		m_nLedFlag[i]++;
		switch (m_nLedStatus[i]) {
		case 1:
			if (m_nLedFlag[i] >= LED_SHINE_SLOW_COUNT) {
				m_nLedFlag[i] = 0;
				__SetGpioOn(m_fdLedWork[i], LEDLIGHT);
			} else
				__SetGpioOn(m_fdLedWork[i], !LEDLIGHT);
			break;
		case 2:
			if (m_nLedFlag[i] >= LED_SHINE_QUICK_COUNT) {
				m_nLedFlag[i] = 0;
				__SetGpioOn(m_fdLedWork[i], LEDLIGHT);
			} else
				__SetGpioOn(m_fdLedWork[i], !LEDLIGHT);
			break;
		case 3:
			m_nLedFlag[i] = 0;
			__SetGpioOn(m_fdLedWork[i], LEDLIGHT);
			break;
		default:
			m_nLedFlag[i] = 0;
			__SetGpioOn(m_fdLedWork[i], !LEDLIGHT);
			break;
		}
	}

}
inline void CMotor::__SetGpioOn(int fd, bool on) {
	int ret = ioctl(fd, on ? SET_GPIO_HIGHT : SET_GPIO_LOW);
//	string dev_name = __GetDevName(fd);
//	if (ret < 0)
//		handle_err_log("%s %s", dev_name.c_str(), "ioctl");
}
inline int CMotor::SetBMQ(bool on)
{
	return ioctl(m_fdBmpSw,on ? SET_GPIO_HIGHT : SET_GPIO_LOW);
}
#endif /* CMOTOR_H_ */
