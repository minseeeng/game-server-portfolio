#pragma once
#include <WinSock2.h>
#include <WS2tcpip.h>

#pragma comment(lib,"ws2_32.lib")
#define CLIENT_MAX 63

class ClientContext;

//접속을 받아서 클라에게 넘긴다
class Server
{
public:
	Server();
	~Server();

	bool CreateSocket();
	void SetAddress(const char* ip, unsigned short port);
	bool Bind();
	bool Listen();
	void Run();
	void CleanUp();
	void BroadCast(unsigned short type, const char* nickname, const char* msg, int len);
	void RemoveClient(ClientContext* context);

private:
	bool Accept(SOCKET& clientSocket, SOCKADDR_IN& clientAddr);

private:
	SOCKET m_listenSocket;
	SOCKADDR_IN m_serverAddr;
	ClientContext* m_clients[CLIENT_MAX];
	int m_clientCount;

	fd_set m_reads;
};

