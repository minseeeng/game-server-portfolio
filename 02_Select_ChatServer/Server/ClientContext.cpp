#include "ClientContext.h"
#include <stdio.h>
#include "Server.h"

ClientContext::ClientContext(SOCKET sock, Server* server)
	: m_clientSocket(sock), m_server(server), m_recvSize(0)
{
	m_buffer[0] = '\0';
	m_nickname[0] = '\0';
}

ClientContext::~ClientContext()
{
	closesocket(m_clientSocket);
}

bool ClientContext::Send(const char * packet, int size)
{
	if (send(m_clientSocket, packet, size, 0) == SOCKET_ERROR)
	{
		printf("send failed! \n");
		return false;
	}

	return true;
}

bool ClientContext::Recv()
{
	int len = recv(m_clientSocket, m_buffer + m_recvSize, sizeof(m_buffer) - m_recvSize, 0);
	if (len <= 0)
		return false;

	m_recvSize += len;

	while (true)
	{
		if (m_recvSize < HEADER_SIZE)
			break;
			
		unsigned short size = *(unsigned short*)m_buffer;
		unsigned short type = *(unsigned short*)(m_buffer+2);

		if (size < HEADER_SIZE || size > BUF_SIZE)
		{
			printf("잘못된 패킷 \n");
			return false;
		}

		if (m_recvSize < size)
			break;

		ProcessPacket(type, m_buffer + HEADER_SIZE, size - HEADER_SIZE);

		m_recvSize -= size;
		memmove(m_buffer, m_buffer + size, m_recvSize);
	}

	return true;
}


void ClientContext::ProcessPacket(unsigned short type, const char* payload, int len)
{

	if(type==TYPE_NICK)
	{
		if (len > NICK_SIZE - 1)
			return;

		strncpy_s(m_nickname, sizeof(m_nickname), payload, len);
		printf("%s 입장 \n", m_nickname);
	}
	else if (type == TYPE_MSG)
	{
		if (len > BUF_SIZE - HEADER_SIZE - NICK_SIZE)
			return;

		printf("%s : %.*s \n", m_nickname, len, payload);
		m_server->BroadCast(type, m_nickname, payload, len);
	}
}
