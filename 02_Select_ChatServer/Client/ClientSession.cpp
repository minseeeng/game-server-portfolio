#include "ClientSession.h"
#include <stdio.h>

ClientSession::ClientSession()
	:m_socket(INVALID_SOCKET), m_addr(), m_recvSize(0)
{
}

ClientSession::~ClientSession()
{
	CleanUp();
}

bool ClientSession::CreateSocket()
{
	m_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (m_socket == INVALID_SOCKET)
	{
		printf("failed socket\n");
		return false;
	}

	return true;
}

void ClientSession::SetAddress(const char* ip, unsigned short port)
{
	memset(&m_addr, 0, sizeof(m_addr));
	m_addr.sin_family = AF_INET;
	inet_pton(AF_INET, ip, &m_addr.sin_addr);
	m_addr.sin_port = htons(port);
}

bool ClientSession::Connect()
{
	if (connect(m_socket, (SOCKADDR*)&m_addr, sizeof(m_addr)) == SOCKET_ERROR)
	{
		printf("failed connect : %u \n", WSAGetLastError());
		return false;
	}

	return true;
}

void ClientSession::CleanUp()
{
	if (m_socket != INVALID_SOCKET)
	{
		closesocket(m_socket);
		m_socket = INVALID_SOCKET;
	}
}

bool ClientSession::Send(unsigned short type, const char* msg)
{
	char packet[BUF_SIZE];

	unsigned short dataSize = (unsigned short)strlen(msg);
	unsigned short size = HEADER_SIZE + dataSize;

	*(unsigned short*)packet = size;
	*(unsigned short*)(packet + 2) = type;
	memcpy(packet + HEADER_SIZE, msg, dataSize);

	if (send(m_socket, packet, size, 0) == SOCKET_ERROR)
	{
		printf("send failed! \n");
		return false;
	}

	return true;
}

bool ClientSession::StartRecvThread()
{
	HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, RecvThread, this, 0, NULL);
	if (hThread == 0)
		return false;

	CloseHandle(hThread);
	return true;
}

void ClientSession::Recv()
{
	char payload[BUF_SIZE];

	while (true)
	{
		int len = recv(m_socket, m_buffer + m_recvSize, sizeof(m_buffer) - m_recvSize, 0);
		if (len <= 0)
			break;

		m_recvSize += len;

		while(true)
		{
			if (m_recvSize < HEADER_SIZE)
				break;

			unsigned short size = *(unsigned short*)m_buffer;
			unsigned short type = *(unsigned short*)(m_buffer + 2);

			if (size < HEADER_SIZE || size > BUF_SIZE)
			{
				printf("잘못된 패킷 \n");
				return;
			}

			if (m_recvSize < size)
				break;

			memcpy(payload, m_buffer + HEADER_SIZE, size - HEADER_SIZE);
			payload[size - HEADER_SIZE] = '\0';

			ProcessPacket(type, payload);

			m_recvSize -= size;
			memmove(m_buffer, m_buffer + size, m_recvSize);
		}

	}
}

void ClientSession::ProcessPacket(unsigned short type, const char* payload)
{
	if (type == TYPE_MSG)
		printf("%s : %s\n", payload, payload + NICK_SIZE);
}

unsigned int __stdcall ClientSession::RecvThread(void* arg)
{
	ClientSession* session = (ClientSession*)arg;
	session->Recv();

	printf("서버 연결 종료\n");
	return 0;
}
