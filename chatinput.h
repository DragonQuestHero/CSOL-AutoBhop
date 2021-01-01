#ifndef _CHATINPUT_
#define _CHATINPUT_

extern bool bInputActive;
extern bool SetKeyboardFocus;
extern int iInputMode;
extern char InputBuf[256];

void DrawChatInputWindow(int buttonsize);

#endif