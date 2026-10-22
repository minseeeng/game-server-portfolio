#pragma once
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <process.h>

#pragma comment(lib,"ws2_32.lib")

#define BUF_SIZE 255
#define HEADER_SIZE 4

#define TYPE_NICK 1
#define TYPE_MSG 2
#define NICK_SIZE 10

class ClientSession
{
public:
	ClientSession();
	~ClientSession();

	bool CreateSocket();
	void SetAddress(const char* ip, unsigned short port);
	bool Connect();
	void CleanUp();
	
	bool Send(unsigned short type, const char* msg);
	bool StartRecvThread();

private:
	void ProcessPacket(unsigned short type, const char* payload);
	void Recv();
	static unsigned int __stdcall RecvThread(void* arg);

private:
	SOCKET m_socket;
	SOCKADDR_IN m_addr;
	char m_buffer[BUF_SIZE];
	int m_recvSize;
};

