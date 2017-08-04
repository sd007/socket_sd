#include <iostream>
#include "OeasySocket.h"
using namespace std;
using namespace Oeasy;
#define SERVERPORT 50101
int main()
{
	OeasySocket::initEnv();
#if 1 //client
	cout<<"Client Started"<<endl;
	char *recvBuf = new char[1024];
	memset(recvBuf, 0, 1024);
	char sendBuf[128] = "this is test from sd";
	OeasySocket *clientsock = new OeasySocket;
	clientsock->CreatSocket(AF_INET, SOCK_STREAM);
	bool bret= clientsock->Connect("192.168.220.128", SERVERPORT);
	clientsock->SetNonBlock(true);
	int sret = clientsock->Writen(clientsock->Getsocket(), sendBuf, strlen(sendBuf)+1);
	if (sret == -1)
	{
		cout<<"write error!!"<<endl;
	}
	cout<<"send msg:"<<sendBuf<<endl;
	while (bret)
	{
		fd_set sockSet;
		FD_ZERO(&sockSet);
		FD_SET(clientsock->Getsocket(), &sockSet);
		struct timeval socktimeout;
		socktimeout.tv_sec=60;            
		socktimeout.tv_usec=0;

		int sret = -100;
		while((sret = clientsock->Select(clientsock->Getsocket()+1, &sockSet, NULL, NULL, &socktimeout)) > 0){
			if (FD_ISSET(clientsock->Getsocket(), &sockSet))
			{
				int ret = clientsock->Readn(clientsock->Getsocket(), recvBuf, 32);
				if (ret == -1)
				{
					cout<<"read error!!"<<endl;
					goto End;
				}else{
					cout<<"recv: "<<recvBuf<<endl;
					clientsock->Writen(clientsock->Getsocket(), sendBuf, strlen(sendBuf)+1);
				}
			}

		}

		Sleep(50);
	}
End:
	cout<<"end!"<<endl;
	clientsock->CloseSocket();
	delete clientsock;
	return 0;
#else //server
	cout<<"Server Started"<<endl;
	char *recvBuf = new char[1024];
	memset(recvBuf, 0, 1024);
	char sendBuf[128] = "this is test from server-------";
	OeasySocket *serversock = new OeasySocket;
	serversock->CreatSocket(AF_INET, SOCK_STREAM);
	int bret = serversock->Bind(SERVERPORT);
	int lret = serversock->Listen();
	//服务端尽可能使用SO_REUSEADDR,在绑定之前尽可能调用setsockopt来设置SO_REUSEADDR套接字选项。该选项可以使得server不必等待TIME_WAIT状态消失就可以重启服务器
	int on = 1;  
	serversock->Setsockopt(serversock->Getsocket(),SOL_SOCKET,SO_REUSEADDR, &on,sizeof(on));

	int acceptSocket = -1;
	while(!serversock->Accept(&acceptSocket)){
		Sleep(50);	
	}
	cout<<"accepted:"<<acceptSocket<<endl;	
	int wret =serversock->Writen(acceptSocket, sendBuf, strlen(sendBuf)+1);
	if (wret == -1)
	{
		cout<<"write error!!"<<endl;
	}
	cout<<"server send msg:"<<wret<<sendBuf<<endl;
	while(1)
	{
		serversock->SetNonBlock(true);
		fd_set sockSet;
		FD_ZERO(&sockSet);
		FD_SET(acceptSocket, &sockSet);
		struct timeval socktimeout;
		socktimeout.tv_sec=60;            
		socktimeout.tv_usec=0;
		while(serversock->Select(acceptSocket+1, &sockSet, NULL, NULL, &socktimeout) > 0){
			if (FD_ISSET(acceptSocket, &sockSet))
			{
				int ret = serversock->Readn(acceptSocket, recvBuf, 21);
				if (ret == -1)
				{
					cout<<"read error!!"<<endl;
					goto End;
				}else{
					cout<<"recv: "<<recvBuf<<endl;
					serversock->Writen(acceptSocket, sendBuf, strlen(sendBuf)+1);
				}
			}

		}
		Sleep(50);
	}
End:
	serversock->CloseSocket(acceptSocket);
	delete serversock;
	OeasySocket::deinitEnv();
	return 0;
#endif

}