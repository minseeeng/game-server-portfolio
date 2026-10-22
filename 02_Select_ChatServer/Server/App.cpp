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
		printf("failed startup\n");
		return false;
	}

	return true;
}

bool CApp::Start()
{
	if ((m_server.CreateSocket() == false))
		return false;

	m_server.SetAddress("127.0.0.1", 30002);

	if ((m_server.Bind() == false))
		return false;

	if ((m_server.Listen() == false))
		return false;

	return true;
}

void CApp::Run()
{
	m_server.Run();
}

void CApp::End()
{
	m_server.CleanUp();
	WSACleanup();
}
