#include "App.h"
#include <stdio.h>

CApp::CApp()
{
}

CApp::~CApp()
{
}

bool CApp::Init()
{
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		printf("failed starttup\n");
		return false;
	}

	return true;
}

bool CApp::Start()
{
	if ((m_session.CreateSocket() == false))
		return false;

	m_session.SetAddress("127.0.0.1", 30002);

	if ((m_session.Connect() == false))
		return false;

	return true;
}

bool CApp::SendNickName()
{
	char nickname[BUF_SIZE];

	while(true)
	{
		printf("닉네임 : ");
		gets_s(nickname, BUF_SIZE - HEADER_SIZE);

		if (strlen(nickname) > NICK_SIZE - 1)
		{
			printf("9자 이하로 입력하세요 \n");
			continue;
		}
		else
			break;
	}

	return m_session.Send(TYPE_NICK, nickname);
}

void CApp::Run()
{
	if ((m_session.StartRecvThread() == false))
		return;

	char msg[BUF_SIZE];
	while(true)
	{
		gets_s(msg, BUF_SIZE - HEADER_SIZE - NICK_SIZE);

		if (m_session.Send(TYPE_MSG, msg) == false)
			return;
	}
}

void CApp::End()
{
	m_session.CleanUp();
	WSACleanup();
}
