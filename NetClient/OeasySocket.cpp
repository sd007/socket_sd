#include "OeasySocket.h"
#include <iostream>
using namespace std;

#ifndef _WIN32
void sigHandler(int signo)  
{  
	while (waitpid(-1, NULL, WNOHANG) > 0)  
		;  
}  
#endif

void Oeasy::OeasySocket::initEnv()
{
#ifdef _WIN32
	//��ʼ��WSA
	WORD sockVersion = MAKEWORD(2,2);
	WSADATA wsaData; 
	if(WSAStartup(sockVersion, &wsaData)!=0)
	{
		printf("WSAStartup error��");
	}
#endif
}
void Oeasy::OeasySocket::deinitEnv()
{
#ifdef _WIN32
	WSACleanup();
#endif
}

Oeasy::OeasySocket::OeasySocket()
{
	m_socket = 0;
	m_bindPort = 0;
#ifndef _WIN32
	signal(SIGCHLD, sigHandler);  
	signal(SIGPIPE, SIG_IGN); 
#endif
}

Oeasy::OeasySocket::~OeasySocket()
{
	closesocket(m_socket);
}

bool Oeasy::OeasySocket::SetNonBlock(bool nonBlock /*= true*/ )
{
#ifdef _WIN32
	unsigned long mode = nonBlock; //mode: 1-������ģʽ   0-����ģʽ
	int ret = ioctlsocket(m_socket,FIONBIO,(unsigned long *)&mode);//���óɷ�����ģʽ��
	if (ret == -1)
	{
		return false;
	}
	return true;
#else
	int opt = fcntl(m_socket, F_GETFL, 0);  
	if (opt == -1)  
		return false;  
	if (nonBlock)  
		opt |= O_NONBLOCK;  // ���ϡ���������flag
	else  
		opt &= ~O_NONBLOCK;  
	if (fcntl(m_socket, F_SETFL, opt) == -1)  
		return false;  
	return true; 
#endif

}

int Oeasy::OeasySocket::CreatSocket( int af, int type, int protocol )
{
	m_socket= socket(af,type, protocol);
	if (m_socket <= 0)
	{
		m_socket = socket(af,type, protocol);
	}
	return m_socket;
}

int Oeasy::OeasySocket::Bind(unsigned short port )
{
	if (port <= 1024)
	{
		return -1;
	}
	m_bindPort = port;
	sockaddr_in serAddr;
	serAddr.sin_family = AF_INET;
	serAddr.sin_port = htons(port);
	serAddr.sin_addr.S_un.S_addr = INADDR_ANY;
	if(bind(m_socket, (sockaddr *)&serAddr, sizeof(serAddr)) == SOCKET_ERROR){
		return -1;
	}
	return 0;
}

int Oeasy::OeasySocket::Listen( int backlog )
{
	return listen(m_socket, backlog);
}

bool Oeasy::OeasySocket::Connect( char* ip, unsigned short port )
{
	struct sockaddr_in servaddr;
	servaddr.sin_port = htons(port);
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr(ip);
	int ret = connect(m_socket, (const struct sockaddr *)&servaddr, sizeof(sockaddr));
	if (ret == -1)
	{
		cout<<"connect failed-"<<errno;
		return false;
	}
	return true;
}

bool Oeasy::OeasySocket::Accept(int *acceptSocket)
{
	*acceptSocket = accept(m_socket, NULL, NULL);
	if (*acceptSocket == -1)  
		return false;  
	return true;  
}

bool Oeasy::OeasySocket::CloseSocket(int socket)
{
	if (socket == 0)
	{
		closesocket(m_socket);
	}else{
		closesocket(socket);
	}
	return true;
}

int Oeasy::OeasySocket::Readn( int socket, char* buf, int len )
{
	char *tempbuf = buf;
	int nleft = len;
	int nread = 0;
	while(nleft > 0){
		if((nread = recv(socket, tempbuf, nleft, 0)) <= 0 ){
			if (errno == EINTR)
			{
				 //�����ȡ�����Ǳ��źŴ����, ��˵�������Լ����� 
				continue;
			}else{
				// 
				cout<<"erro = "<<GetLastError()<<endl;
				//GetLastError()����-10053�������������ݿɶ�
				return -1;
			}
		}
		else if(nread == 0)  //û���ݿɶ���
		{
			return len-nleft; 
		}		
		nleft -= nread;
		tempbuf += nread;
	}
	return len;
}

int Oeasy::OeasySocket::Writen( int socket, char* buf, int len )
{
	int nwritten = 0;
	int nleft = len;
	char *tempbuf = buf;
	while(nleft > 0){
		if ((nwritten = send(socket, tempbuf, nleft, 0)) <= 0)
		{
			if (errno == EINTR)
			{
				continue;
			}else{
				return -1;
			}
		}
		//��� ==0��˵����ʲôҲûд��, ���Լ���д  
		else if (nwritten == 0)  
			continue;  
		nleft -= nwritten;
		tempbuf += nwritten;
	}
	return len;
}

int Oeasy::OeasySocket::Getsocket()
{
	return m_socket;
}

int Oeasy::OeasySocket::Setsockopt( int sockfd, int level, int optname, const void *optval, int optlen )
{
	//����˾�����ʹ��SO_REUSEADDR,�ڰ�֮ǰ�����ܵ���setsockopt������SO_REUSEADDR�׽���ѡ���ѡ�����ʹ��server���صȴ�TIME_WAIT״̬��ʧ�Ϳ�������������
	//int on = 1;  
	//if (setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,  
	//	&on,sizeof(on)) == -1)
	return setsockopt(sockfd, level, optname,  (const char*)optval, optlen); 
}

int Oeasy::OeasySocket::Select( int nfds, fd_set* readfds, fd_set* writefds, fd_set* exceptfds, struct timeval* timeout )
{
	return select(nfds, readfds, writefds, exceptfds, timeout);
}






