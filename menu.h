#ifndef _IMGUIMENU_
#define _IMGUIMENU_

typedef struct
{
	int numhitbox;
	char displaymodel[MAX_QPATH];
	char checkmodel[MAX_QPATH];
} model_aim_select_t;
extern deque<model_aim_select_t> Model_Aim_Select;

typedef struct
{
	int numhitboxes;
	char displaymodel[MAX_QPATH];
	char checkmodel[MAX_QPATH];
} model_aim_t;
extern deque<model_aim_t> Model_Aim;

extern float radiusy;
extern bool changewindowfocus;
extern GLuint texture_id[2048];
extern bool keysmenu[256];
extern bool bShowMenu;
char* KeyEventChar(int Key);
void DrawMenuWindow();
void EaseMenu(float& menuspeed, int index, float windowsize, int speed, bool showmenu);
bool SliderFloat(const char* label, float* v, float v_min, float v_max, const char* format = "%.1f");

#endif