#ifndef _IDHOOK_
#define _IDHOOK_

class IdHook
{
public:
	int FirstKillPlayer[33];
	void ClearPlayer();
	void AddPlayer(int i);
	void First_Kill_Mode();
};

extern IdHook idhook;

#endif