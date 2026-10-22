#pragma once
#include "ClientSession.h"

class CApp
{
public:
	CApp();
	~CApp();

	bool Init();
	bool Start();
	bool SendNickName();
	void Run();
	void End();
	
private:
	ClientSession m_session;
};

