#ifndef _CROSSHAIR_
#define _CROSSHAIR_

typedef struct
{
	float angle;
	int iBarSize;
	float flCrosshairDistance;
} crosshair_t;
extern deque<crosshair_t> CrosshairDraw;

void DrawCrossHair();
void CrossHair();

#endif