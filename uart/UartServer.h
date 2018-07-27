/**
 * File name: UartServer.h
 * Author: Even
 * Email: 871454583@qq.com
 * Version: 1.0
 * Created on: 2017年8月31日
 * Description: UART数据收发
 */

#ifndef UARTSERVER_H_
#define UARTSERVER_H_

#include <fcntl.h>
#include <sys/ioctl.h>
#include "../common.h"
#include "../sys/serial.h"
#include "../sys/config.h"
#include "UartCache.h"
#include "../utils/DevUtils.h"

#define DEV_UART3 "/dev/ttySP3"//连接FPGA
#define DEV_UART1 "/dev/ttySP1"//连接温度模块
#define DEV_UART4 "/dev/ttySP4"//连接电机保护模块

//#include "../socket/SocketServer.h"
class UartServer: IOnFindUartListener {
public:
	UartServer();
	~UartServer();
	void Start();

	void GetUartData(int fd , uchar* buf,int* lenofdata);
	int UartSendByfd(int fd,uchar* buf,int len);
	void SendMsg(Msg *msg);
	virtual void onFindUartInstruction(int fd, Msg *msg, void *args);
	void SetGetDownMsgListener(IOnGetDownCmdListener * listener);
	void SendtoFPGA(uint8_t dir,int diffdegree,int currentdegree);
	int m_fdFPGA;
//	int m_fdTEMP,m_fdPROT;
	//	int m_bUartEn;

private:
	uint8_t mSendBuf[BUFFER_SIZE];
	uint8_t mRecvBuf[BUFFER_SIZE];
	int mSendLen;

	int datalen;
	UartCache* mUartCache;//数据处理类
	IOnGetDownCmdListener *mGetDownCmdListener;
	uint8_t mSendtoFPGA[17];

	// 串口初始化
//	void __InitUART();
	void __UpstreamProcessor(int fd,Msg *msg, void *args);
	void __DownstreamProcessor(int fd, Msg *msg, void *args);
};

//inline UartServer::UartServer():mGetDownCmdListener(NULL),mSendtoFPGA({0x24,0x24,0x51,0x0a,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0d})
inline UartServer::UartServer()
	{
		mSendLen=0;
		datalen=0;
		mUartCache = new UartCache(this);
		mGetDownCmdListener=NULL;
		mSendtoFPGA={0x24,0x24,0x51,0x0a,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0d};
		int ret;
		m_fdFPGA = DevUtils::OpenDev(DEV_UART3, O_RDWR | O_NOCTTY);
		ret = set_port_attr(m_fdFPGA, UART_BAUD_RATE, UART_DATA_BIT, UART_STOP_BIT, UART_PARITY,
				UART_VTIME, UART_VMIN);
		if (ret < 0) {
			printf("set uartfpga attr failed m_fdFPGA = %d\n",m_fdFPGA);
			exit(-1);
		}
//	m_fdTEMP = open(DEV_UART1, O_RDWR | O_NOCTTY);
//	ret = set_port_attr(m_fdTEMP, UART_BAUD_RATE, UART_DATA_BIT, UART_STOP_BIT, UART_PARITY,
//			UART_VTIME, UART_VMIN);
//	if (ret < 0) {
//		printf("set uarttemp attr failed \n");
//		exit(-1);
//	}
//	m_fdPROT = open(DEV_UART4, O_RDWR | O_NOCTTY);
//	ret = set_port_attr(m_fdPROT, UART_BAUD_RATE, UART_DATA_BIT, UART_STOP_BIT, UART_PARITY,
//			UART_VTIME, UART_VMIN);
//	if (ret < 0) {
//		printf("set uartprot attr failed \n");
//		exit(-1);
//	}

	//TODO  配置为非阻塞模式
//	if(fcntl(m_fdFPGA,F_SETFL,FNDELAY) < 0)
//		printf("fcntl m_fdDEBUG failed\n");
//
//	if(fcntl(m_fdTEMP,F_SETFL,FNDELAY) < 0)
//		printf("fcntl m_fdLIGHT1 failed\n");
//
//	if(fcntl(m_fdPROT,F_SETFL,FNDELAY) < 0)
//		printf("fcntl m_fdLIGHT2 failed\n");
//

	printf("UartServer Object created!\n");

}

inline UartServer::~UartServer(){
	int ret;
//	ret = close(m_fdPROT);
//	if (ret)
//		handle_error_en(ret, DEV_UART1);
//	ret = close(m_fdTEMP);
//	if (ret)
//		handle_error_en(ret, DEV_UART1);
	ret = close(m_fdFPGA);
	if (ret)
		handle_error_en(ret, DEV_UART1);
	printf("UartServer Object destroyed!\n");
}
inline void UartServer::Start() {
//	while(true)
//	{
			this->GetUartData(m_fdFPGA,mRecvBuf,&datalen);
			mUartCache->MsgPreParse(m_fdFPGA,mRecvBuf, datalen, NULL);

//	}
}
inline void UartServer::SendtoFPGA(uint8_t dir,int diffdegree,int currentdegree){
	mSendtoFPGA[4]=dir;
	diffdegree *= 10;
	mSendtoFPGA[8]=diffdegree/256;
	mSendtoFPGA[9]=diffdegree%256;
	currentdegree *= 10;
	mSendtoFPGA[12]=currentdegree/256;
	mSendtoFPGA[13]=currentdegree%256;
	this->UartSendByfd(m_fdFPGA,mSendtoFPGA,17);
	printf("send to FPGA data :");
	for(int i =0 ;i<17;i++)
	{
		printf("%x ",mSendtoFPGA[i]);
	}
	printf("\n");

}
//TODO 发送给FPGA
inline int UartServer::UartSendByfd(int fd,uchar* buf,int len){
	if(fd>0)
		return write(fd, buf, len);
	else
		return -1;
}
inline void UartServer::GetUartData(int fd, uchar* buf,int* lenofdata) {

		*lenofdata = read(fd,buf, BUFFER_SIZE);
		if(*lenofdata > 0)
		{
//			printf("recive FPGA data :");
//			for(int i =0 ;i<*lenofdata;i++)
//			{
//				printf("%x ",*(buf+i));
//			}
//			printf("\n");
			;
		}

}

inline void UartServer::SetGetDownMsgListener(IOnGetDownCmdListener *listener) {
	this->mGetDownCmdListener = listener;
}

inline void UartServer::SendMsg(Msg *msg) {
	this->__UpstreamProcessor(m_fdFPGA,msg, NULL);
}
inline void UartServer::onFindUartInstruction(int fd, Msg *msg, void *args) {
//	log("Parse 1 instruction.cmd:%x dataLen:%d isRecv:%d", msg->cmd, msg->dataLen, msg->isRecv);
	if (msg->isRecv) {
		this->__DownstreamProcessor(fd, msg, args);
	} else {
//		__UpstreamProcessor(fd, msg, args);
	}
}


inline void UartServer::__UpstreamProcessor(int fd,Msg *msg, void *args) {
	uint8_t sdbuf[SOCKET_MSG_DATA_SIZE];
	int len = msg->CopyToBuf(sdbuf, 0);
	int ret;
	printf("upstream:");
	for (int i = 0; i < len; i++) {
		printf("%x ", sdbuf[i]);
	}
	printf("\n");
	if(fd<=0)
	{
		printf("ERROR: Uart not connect!\n");
		return ;
	}
	if ((ret = send(fd, sdbuf, len, 0)) == -1) {
		printf("ERROR: Failed  to sent string.\n");
		close(fd);
		exit(1);
	}
	printf("OK: Sent %d bytes successful, please enter again.\n", ret);
}
inline void UartServer::__DownstreamProcessor(int fd, Msg *msg, void *args) {
	Msg *backMsg = NULL;
	if (mGetDownCmdListener != NULL) {
		backMsg = mGetDownCmdListener->onGetDownCmd(msg);
		if (backMsg != NULL) {
			mSendLen += backMsg->CopyToBuf(mSendBuf, mSendLen);
			delete backMsg;
		}
	}
}
#endif
