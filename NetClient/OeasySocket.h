#ifndef OEASY_SOKET
#define OEASY_SOKET
#include <stdio.h>

#ifdef _WIN32
#include <WinSock2.h>
#include <errno.h>
#include <fcntl.h>
#pragma  comment(lib,"ws2_32.lib")
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h> 
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <arpa/inet.h>
#endif


#ifdef _WIN32
#define  SLEEP(m) Sleep(m)
#define  closesocket(s) closesocket(s)
#else
#define  SLEEP(m) usleep((m*1000))
#define  closesocket(s) close(s)
#endif

namespace Oeasy{

class OeasySocket{
public:
	static void initEnv();
	static void deinitEnv();
	OeasySocket();
	~OeasySocket();
	int Getsocket();
	int CreatSocket(int af, int type, int protocol = 0);
	int Bind(unsigned short port);
	int Listen(int backlog = 128);
	int Setsockopt(int sockfd, int level, int optname,  const void *optval, int optlen); 
	bool CloseSocket(int socket = 0);

	bool Connect(char* ip, unsigned short port);
	//acceptSocket为accept返回的数据socket
	bool Accept(int* acceptSocket);

	int Writen(int sockfd, char* buf, int len);
	int Readn(int sockfd, char* buf, int len);

	int Select(int nfds, fd_set* readfds, fd_set* writefds, fd_set* exceptfds, struct timeval* timeout);
	bool SetNonBlock(bool nonBlock = true);
private:
	int m_socket;
	short m_bindPort;
};

};

#endif

