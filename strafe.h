#ifndef _STRAFE_
#define _STRAFE_

typedef struct
{
	Vector Pos1;
	Vector Pos2;
} strafe_t;
extern deque<strafe_t> StrafeDraw;

extern bool Strafe;
extern bool Fastrun;
extern bool Gstrafe;
extern bool Bhop;
extern bool Jumpbug;

void Kz(float frametime, struct usercmd_s *cmd);
void DrawKzWindows();
void DrawLongJump();
void KzFameCount();
float HeightOrigin();

#endif