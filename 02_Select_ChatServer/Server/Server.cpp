#include "Server.h"
#include <stdio.h>
#include "ClientContext.h"

Server::Server()
	: m_listenSocket(INVALID_SOCKET), m_clientCount(0)
{
}

Server::~Server()
{
	CleanUp();
}

bool Server::CreateSocket()
{
	m_listenSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (m_listenSocket == INVALID_SOCKET)
	{
		printf("failed socket\n");
		return false;
	}

	return true;
}

void Server::SetAddress(const char* ip, unsigned short port)
{
	memset(&m_serverAddr, 0, sizeof(m_serverAddr));
	m_serverAddr.sin_family = AF_INET;
	inet_pton(AF_INET, ip, &m_serverAddr.sin_addr);
	m_serverAddr.sin_port = htons(port);
}

bool Server::Bind()
{
	if (bind(m_listenSocket, (SOCKADDR*)&m_serverAddr, sizeof(m_serverAddr)) == SOCKET_ERROR)
	{
		printf("bind error \n");
		return false;
	}

	return true;
}

bool Server::Listen()
{
	if (listen(m_listenSocket, 5) == SOCKET_ERROR)
	{
		printf("listen error \n");
		return false;
	}

	FD_ZERO(&m_reads);
	FD_SET(m_listenSocket, &m_reads);
	printf("listen start ... \n");
	return true;
}
/*
void Server::Run()
{
	struct timeval timeout;

	while (true)
	{
		fd_set cpyreads = m_reads;

		int eventCount = select(0, &cpyreads, NULL, NULL, NULL);
		if (eventCount == SOCKET_ERROR) break;

		for (int i = 0; i < m_reads.fd_count; i++)
		{
			for (int j = 0; j < cpyreads.fd_count; ++j)
			{
				SOCKET activeSocket = cpyreads.fd_array[j];
				if (activeSocket == m_listenSocket)
				{
					SOCKET clientSocket;
					SOCKADDR_IN clientAddr;

					if ((Accept(clientSocket, clientAddr)) == false)
						continue;

					if (m_clientCount >= CLIENT_MAX)
					{
						printf("client full \n");
						closesocket(clientSocket);
						continue;
					}

					ClientContext* clientContext = new ClientContext(clientSocket, this);
					m_clients[m_clientCount++] = clientContext;
					FD_SET(clientSocket, &m_reads);
				}
				else
				{
					ClientContext* client = m_clients[i];

					if( m_reads.fd_array[i] == activeSocket )
					//if (m_clients[i]->GetSocket() == activeSocket)
					{
						if ((m_clients[i]->Recv()) == false)
						{
							FD_CLR(activeSocket, &m_reads);
							printf("%s 퇴장 \n", client->GetNickname());
							RemoveClient(client);
							delete client;
						}
						break;
					}
				}

			}
		
			//m_reads.clientContexts[i].Update();
		}
	}
}
*/

void Server::Run()
{
	struct timeval timeout;

	while (true)
	{
		fd_set cpyreads = m_reads;

		timeout.tv_sec = 5;
		timeout.tv_usec = 0;

		int eventCount = select(0, &cpyreads, NULL, NULL, &timeout);

		if (eventCount == SOCKET_ERROR)
			break;

		if (eventCount == 0)
		{
			printf("time out! \n");
			continue;
		}

		for (int i = 0; i < m_reads.fd_count; i++)
		{
			SOCKET sock = m_reads.fd_array[i];

			if (!FD_ISSET(sock, &cpyreads))
				continue;

			if (sock == m_listenSocket)
			{
				SOCKET clientSocket;
				SOCKADDR_IN clientAddr;

				if ((Accept(clientSocket, clientAddr)) == false)
					continue;

				if (m_clientCount >= CLIENT_MAX)
				{
					printf("client full \n");
					closesocket(clientSocket);
					continue;
				}

				ClientContext* clientContext = new ClientContext(clientSocket, this);
				m_clients[m_clientCount++] = clientContext;
				FD_SET(clientSocket, &m_reads);
			}
			else
			{
				ClientContext* client = m_clients[i-1];

				if ((client->Recv()) == false)
				{
					FD_CLR(sock, &m_reads);
					printf("%s 퇴장 \n", client->GetNickname());
					RemoveClient(client);
					delete client;
					i--;
				}
			}
		}
	}
}

void Server::CleanUp()
{
	if (m_listenSocket != INVALID_SOCKET)
	{
		closesocket(m_listenSocket);
		m_listenSocket = INVALID_SOCKET;
	}
}

void Server::BroadCast(unsigned short type, const char* nickname, const char* msg, int len)
{
	char packet[BUF_SIZE];
	unsigned short size = HEADER_SIZE + NICK_SIZE + len;

	*(unsigned short*)packet = size;
	*(unsigned short*)(packet + 2) = type;

	memcpy(packet + HEADER_SIZE, nickname, NICK_SIZE);
	memcpy(packet + HEADER_SIZE + NICK_SIZE, msg, len);

	for (int i = 0; i < m_clientCount; i++)
	{
		m_clients[i]->Send(packet, size);
	}
}

void Server::RemoveClient(ClientContext* context)
{
	for (int i = 0; i < m_clientCount; i++)
	{
		if (m_clients[i] == context)
		{
			for (int j = i; j < m_clientCount - 1; j++)
			{
				m_clients[j] = m_clients[j + 1];
			}
			m_clientCount--;
			return;
		}
	}
}

bool Server::Accept(SOCKET& clientSocket, SOCKADDR_IN& clientAddr)
{
	int sizeClientAddr = sizeof(clientAddr);
	clientSocket = accept(m_listenSocket, (SOCKADDR*)&clientAddr, &sizeClientAddr);
	if (clientSocket == INVALID_SOCKET)
	{
		printf("accept error \n");
		return false;
	}
	
	return true;
}

