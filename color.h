#ifndef _COLOR_
#define _COLOR_

extern float color_red;
extern float color_green;
extern float color_blue;

ImColor Red();
ImColor Green();
ImColor Blue();
ImColor White();
ImColor Black();
ImColor Wheel1();
ImColor Wheel2();
ImColor Wheel3();
ImColor Team(int i);

void ColorChange();
void ConsolePrintColor(BYTE R, BYTE G, BYTE B, char* fmt, ...);

typedef struct Color24
{
	BYTE R, G, B;
} TColor24, * PColor24;

#endif