#include "App.h"
#include <stdio.h>

int main()
{
	CApp app;

	if ((app.Init())==false)
		return 0;

	if ((app.Start()) == false)
		return 0;

	if ((app.SendNickName()) == false)
		return 0;

	app.Run();
	app.End();

	return 0;
}