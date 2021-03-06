/**
 * File name: Socket.h
 * Author: JoshuaWong
 * Email: 1034316376@qq.com
 * Version: 1.0
 * Created on: 2017年6月23日 上午10:12:15
 * Description: 
 */

#ifndef SOCKETSERVER_H_
#define SOCKETSERVER_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include "../common.h"
#include "IOnFindMsgListener.h"
#include "IOnGetDownCmdListener.h"
#include "SocketCache.h"
#include "SocketMsg.h"
#include "../sys/watchdog.h"

#define LISTEN_PORT 5000
#define MAXLINE 4096

#define SOCKET_PORT 		5000       		// The port which is communicate with server
#define BACKLOG 	10
#define SOCKET_BUFFER_LEN 512              		// Buffer length

#define DEV_WATCHDOG "/dev/watchdog"		//看门狗

class SocketServer: IOnFindMsgListener {
public:
	SocketServer();
	~SocketServer();
	/*
	 * SOCKET服务启动
	 */
	void Start();
	/*
	 * 正常断开
	 */
	void DisConnect();
	/*
	 * 设置串口开启
	 */
	void SetEnable(bool enable);
	/*
	 * 查询连接状态
	 */
	int CheckStatu();
	/*
	 * 设置侦听函数指针
	 */
	void SetGetDownMsgListener(IOnGetDownCmdListener * listener);
	/*
	 * 以消息体的方式发送
	 */
	void SendMsg(Msg *msg);
	// @Override指令分类执行
	virtual void onFindInstruction(int fd, Msg *msg, void *args);
private:
	uint8_t mSendBuf[BUFFER_SIZE];
	uint8_t mRecvBuf[BUFFER_SIZE];
	int mSendLen;
	DataCache *mSocketCache;
	IOnGetDownCmdListener *mGetDownCmdListener;
	bool m_bUartEn;

	int m_nSocketServerFd, m_nSocketClientFd,m_fdWatchDog;

	// 上行指令处理
	void __UpstreamProcessor(int fd, Msg *msg, void *args);
	// 下行指令处理
	void __DownstreamProcessor(int fd, Msg *msg, void *args);
	//初始化看门狗
	void __InitWatchDog();
	//喂狗
	void __FeedDog();
	//关狗
	void __CloseWd();
};

inline SocketServer::SocketServer() :
		mSendLen(0), mGetDownCmdListener(NULL), m_bUartEn(false), m_nSocketServerFd(-1), m_nSocketClientFd(
				-1),m_fdWatchDog(-1) {
	printf("SocketServer Object created!\n");
	mSocketCache = new DataCache(this);
}

inline SocketServer::~SocketServer() {
	close(m_nSocketServerFd);
	m_nSocketServerFd = -1;

	printf("SocketServer Object destroyed!\n");
}
inline void SocketServer::DisConnect()
{
	close(m_nSocketClientFd);
	m_nSocketClientFd = -1;

}
inline void SocketServer::Start() {
	socklen_t sin_size;                      	// to store struct size
	struct sockaddr_in addr_local;
	struct sockaddr_in addr_remote;

	/* Get the Socket file descriptor */
	if ((m_nSocketServerFd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		printf("ERROR: Failed to obtain Socket Descriptor.\n");
		return;
	} else {
		printf("OK: Obtain Socket Descriptor successfully.\n");
	}

	/* Fill the local socket address struct */
	addr_local.sin_family = AF_INET;				// Protocol Family
	addr_local.sin_port = htons(SOCKET_PORT);         		// Port number
	addr_local.sin_addr.s_addr = htonl(INADDR_ANY);	// AutoFill local address
	memset(addr_local.sin_zero, 0, 8);				// Flush the rest of struct

	/* Bind a special Port */
	if (bind(m_nSocketServerFd, (struct sockaddr*) &addr_local, sizeof(struct sockaddr)) == -1) {
		printf("ERROR: Failed to bind Port %d.\n", SOCKET_PORT);
		return;
	} else {
		printf("OK: Bind the IP %x successfully.\n",addr_local.sin_addr.s_addr);
	}

	/* Listen remote connect/calling */
	if (listen(m_nSocketServerFd, BACKLOG) == -1) {
		printf("ERROR: Failed to listen Port %d.\n", SOCKET_PORT);
		return;
	} else {
		printf("OK: Listening the Port %d successfully.\n", SOCKET_PORT);
	}

	while (1) {
		sin_size = sizeof(struct sockaddr_in);

		/*  Wait a connection, and obtain a new socket file descriptor for single connection */
		if ((m_nSocketClientFd = accept(m_nSocketServerFd, (struct sockaddr *) &addr_remote,
				&sin_size)) == -1) {
			printf("ERROR: Obtain new Socket Descriptor error.\n");
			continue;
		} else {
			printf("OK: Server has got connect from %s.\n", inet_ntoa(addr_remote.sin_addr));
#if WATCH_DOG_EN
	__InitWatchDog();
#endif
		}

		/* Child process */
		while (true) {
			int recvCount = recv(m_nSocketClientFd, mRecvBuf, sizeof(mRecvBuf), 0); // 接收
			if (recvCount == 0) {
				printf("client disconnect\n");
				break;
			}
			if(DEBUG_even)
			{
				printf("recvCount = %d\n\n",recvCount);
				printf("receive data:");
				for (int i = 0; i < recvCount; i++) {
					printf("%x ", mRecvBuf[i]);
				}
				printf("\n");
			}
			__FeedDog();
			mSocketCache->MsgPreParse(m_nSocketClientFd, mRecvBuf, recvCount, NULL);
			mSocketCache->MsgPreParse(m_nSocketClientFd, mSendBuf, mSendLen, NULL);
//			printf("cmd is %d\n",mSendBuf[2]);
			if(mSendBuf[2]==0x10)
			{

				exit(EXIT_SUCCESS);
			}
			memset(mRecvBuf, 0, sizeof(mRecvBuf));
			memset(mSendBuf, 0, sizeof(mSendBuf));
			mSendLen = 0;
			usleep(DATA_PARSE_DELAY_US);
		}
#if WATCH_DOG_EN
		__CloseWd();
#endif
		close(m_nSocketClientFd);
		m_nSocketClientFd = -1;
		while (waitpid(-1, NULL, WNOHANG) > 0)
			;
	}
}

inline void SocketServer::SetEnable(bool enable) {
	this->m_bUartEn = enable;
}
inline int SocketServer::CheckStatu(){
	if(m_nSocketServerFd > 0 && m_nSocketClientFd > 0)
		return 1;
	else
		return 0;
}
inline void SocketServer::SetGetDownMsgListener(IOnGetDownCmdListener *listener) {
	this->mGetDownCmdListener = listener;
}

inline void SocketServer::SendMsg(Msg *msg) {
	__UpstreamProcessor(m_nSocketClientFd, msg, NULL);
}

inline void SocketServer::onFindInstruction(int fd, Msg *msg, void *args) {
//	log("Parse 1 instruction.cmd:%x dataLen:%d isRecv:%d", msg->cmd, msg->dataLen, msg->isRecv);
	if (msg->isRecv) {
		__DownstreamProcessor(fd, msg, args);
	} else {
		__UpstreamProcessor(fd, msg, args);
	}
}

inline void SocketServer::__UpstreamProcessor(int fd, Msg *msg, void *args) {
	uint8_t sdbuf[32];
	int len = msg->CopyToBuf(sdbuf, 0);
	int ret;
	if(DEBUG_even){
		printf("upstream:");
		for (int i = 0; i < len; i++) {
			printf("%x ", sdbuf[i]);
		}
		printf("\n");
	}
	if(fd <=0)
	{
		log("ERROR: Socket not connect!\n");
		return ;
	}
	if ((ret = send(fd, sdbuf, len, 0)) == -1) {
		log("ERROR: Failed  to sent string.\n");
		close(fd);
		exit(1);
	}
	log("OK: Sent %d bytes successful, please enter again.\n", ret);
}

inline void SocketServer::__DownstreamProcessor(int fd, Msg *msg, void *args) {
	Msg *backMsg = NULL;
	if (mGetDownCmdListener != NULL) {
		backMsg = mGetDownCmdListener->onGetDownCmd(msg);
		if (backMsg != NULL) {
			mSendLen += backMsg->CopyToBuf(mSendBuf, mSendLen);
			delete backMsg;
		}
	}
}
inline void SocketServer::__InitWatchDog() {
	int timeout = WATCH_DOG_TIMEOUT;//
	m_fdWatchDog = open(DEV_WATCHDOG, O_WRONLY);
	printf("WDT is opened!\n");
	ioctl(m_fdWatchDog, WDIOC_SETTIMEOUT, &timeout);
	ioctl(m_fdWatchDog, WDIOC_GETTIMEOUT, &timeout);
	printf("The timeout was is %d seconds\n", timeout);
}
inline void SocketServer::__CloseWd() {
	int opt = WDIOS_DISABLECARD;
    if (write(m_fdWatchDog, "V", 1) != 1) {
        printf("write WDT_OK_TO_CLOSE failed!");
    }
	ioctl(m_fdWatchDog, WDIOC_SETOPTIONS, &opt);
	close(m_fdWatchDog);
}
inline void SocketServer::__FeedDog() {
	write(m_fdWatchDog, "\0", 1);
}

#endif /* SOCKETSERVER_H_ */
