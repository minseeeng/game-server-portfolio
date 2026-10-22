#pragma once
#include <WinSock2.h>
#include <WS2tcpip.h>

#pragma comment(lib,"ws2_32.lib")

#define BUF_SIZE 255
#define HEADER_SIZE 4

#define TYPE_NICK 1
#define TYPE_MSG 2
#define NICK_SIZE 10

class Server;

//클라이언트 하나와 통신한다
class ClientContext
{
public:
	ClientContext(SOCKET sock,Server* server);
	~ClientContext();

	bool Send(const char* packet, int size);
	bool Recv();
	SOCKET GetSocket() { return m_clientSocket; }
	const char* GetNickname() { return m_nickname; }

private:
	void ProcessPacket(unsigned short type, const char* payload, int len);

private:
	SOCKET m_clientSocket;
	Server* m_server;
	char m_buffer[BUF_SIZE];
	char m_nickname[NICK_SIZE];
	int m_recvSize;
};

