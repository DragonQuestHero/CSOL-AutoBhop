#ifndef _CLIENT_
#define _CLIENT_

#pragma once

#pragma warning(disable:4005)
#pragma warning(disable:4018)
#pragma warning(disable:4244)
#pragma warning(disable:4715)
#pragma warning(disable:4819)
#pragma warning(disable:4996)
#pragma warning(disable:4700)
#pragma warning(disable:4305)
#pragma warning(disable:4410)
#pragma warning(disable:4409)
#pragma warning(disable:4002)

#define _USE_MATH_DEFINES
#define IMGUI_DEFINE_MATH_OPERATORS

#include <ctime>
#include <map>
#include <vector>
#include <fstream>
#include <windows.h>
#include <iostream>
#include <array>
#include <deque>
#include <math.h>
#include <list>

using namespace std;
extern char hackdir[256];
extern char hldir[256];

#include "bass/dll.h"
#include "bass/bass.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_opengl2.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_internal.h"
#include "imgui/soil.h"
#include "imgui/gl.h"
#include "imgui/glu.h"

#include "vector/vector.h"
#include "vector/qangle.h"

#include "SDK/engine/wrect.h"
#include "SDK/engine/cl_dll.h"
#include "SDK/engine/cdll_int.h"
#include "SDK/engine/entity_types.h"
#include "SDK/engine/pmtrace.h"
#include "SDK/engine/pm_defs.h"
#include "SDK/engine/pm_movevars.h"
#include "SDK/engine/r_efx.h"
#include "SDK/engine/studio.h"
#include "SDK/engine/studio_event.h"
#include "SDK/engine/triangleapi.h"
#include "SDK/engine/cvardef.h"
#include "SDK/common/com_model.h"
#include "SDK/common/event_api.h"
#include "SDK/common/ref_params.h"
#include "SDK/common/screenfade.h"
#include "SDK/common/net_api.h"
#include "SDK/common/con_nprint.h"
#include "SDK/misc/r_studioint.h"
#include "SDK/misc/steamtypes.h"

#include "resources/detours.h"
#include "resources/defs.h"
#include "resources/stringfinder.h"
#include "resources/trace.h"
#include "resources/textures.h"
#include "resources/parsemsg.h"
#include "resources/keydefs.h"
#include "resources/gl3winit.h"

#include "opengl.h"
#include "usermsg.h"
#include "offset.h"
#include "soundesp.h"
#include "weapons.h"
#include "player.h"
#include "cvar.h"
#include "color.h"
#include "strafe.h"
#include "idhook.h"
#include "autoroute.h"
#include "gui.h"
#include "esp.h"
#include "menu.h"
#include "snapshot.h"
#include "aimbot.h"
#include "norecoil.h"
#include "nospread.h"
#include "studiomodelrender.h"
#include "skins.h"
#include "sound.h"
#include "hitboxes.h"
#include "fov.h"
#include "crosshair.h"
#include "tracegrenade.h"
#include "flash.h"
#include "studio.h"
#include "modechange.h"
#include "guiwindow.h"
#include "antiaim.h"
#include "fakelag.h"
#include "chatinput.h"
#include "killsound.h"
#include "steamid.h"
#include "lightmap.h"
#include "wallskin.h"
#include "sky.h"
#include "spawn.h"
#include "model.h"
#include "radar.h"
#include "strings.h"

void HookClientFunctions();
void CL_Move();
void bSendpacket(bool status);
void AdjustSpeed(double speed);

//typedef struct cl_clientfuncs_s
//{
//	int(*Initialize) (cl_enginefunc_t *pEnginefuncs, int iVersion);
//	int(*HUD_Init) (void);
//	int(*HUD_VidInit) (void);
//	void(*HUD_Redraw) (float time, int intermission);
//	int(*HUD_UpdateClientData) (client_data_t *pcldata, float flTime);
//	int(*HUD_Reset) (void);
//	void(*HUD_PlayerMove) (struct playermove_s *ppmove, int server);
//	void(*HUD_PlayerMoveInit) (struct playermove_s *ppmove);
//	char(*HUD_PlayerMoveTexture) (char *name);
//	void(*IN_ActivateMouse) (void);
//	void(*IN_DeactivateMouse) (void);
//	void(*IN_MouseEvent) (int mstate);
//	void(*IN_ClearStates) (void);
//	void(*IN_Accumulate) (void);
//	void(*CL_CreateMove) (float frametime, struct usercmd_s *cmd, int active);
//	int(*CL_IsThirdPerson) (void);
//	void(*CL_CameraOffset) (float *ofs);
//	struct kbutton_s *(*KB_Find) (const char *name);
//	void(*CAM_Think) (void);
//	void(*V_CalcRefdef) (struct ref_params_s *pparams);
//	int(*HUD_AddEntity) (int type, struct cl_entity_s *ent, const char *modelname);
//	void(*HUD_CreateEntities) (void);
//	void(*HUD_DrawNormalTriangles) (void);
//	void(*HUD_DrawTransparentTriangles) (void);
//	void(*HUD_StudioEvent) (const struct mstudioevent_s *event, const struct cl_entity_s *entity);
//	void(*HUD_PostRunCmd) (struct local_state_s *from, struct local_state_s *to, struct usercmd_s *cmd, int runfuncs, double time, unsigned int random_seed);
//	void(*HUD_Shutdown) (void);
//	void(*HUD_TxferLocalOverrides) (struct entity_state_s *state, const struct clientdata_s *client);
//	void(*HUD_ProcessPlayerState) (struct entity_state_s *dst, const struct entity_state_s *src);
//	void(*HUD_TxferPredictionData) (struct entity_state_s *ps, const struct entity_state_s *pps, struct clientdata_s *pcd, const struct clientdata_s *ppcd, struct weapon_data_s *wd, const struct weapon_data_s *pwd);
//	void(*Demo_ReadBuffer) (int size, unsigned char *buffer);
//	int(*HUD_ConnectionlessPacket) (struct netadr_s *net_from, const char *args, char *response_buffer, int *response_buffer_size);
//	int(*HUD_GetHullBounds) (int hullnumber, float *mins, float *maxs);
//	void(*HUD_Frame) (double time);
//	int(*HUD_Key_Event) (int down, int keynum, const char *pszCurrentBinding);
//	void(*HUD_TempEntUpdate) (double frametime, double client_time, double cl_gravity, struct tempent_s **ppTempEntFree, struct tempent_s **ppTempEntActive, int(*Callback_AddVisibleEntity)(struct cl_entity_s *pEntity), void(*Callback_TempEntPlaySound)(struct tempent_s *pTemp, float damp));
//	struct cl_entity_s *(*HUD_GetUserEntity) (int index);
//	int(*HUD_VoiceStatus) (int entindex, qboolean bTalking);
//	int(*HUD_DirectorMessage) (unsigned char command, unsigned int firstObject, unsigned int secondObject, unsigned int flags);
//	int(*HUD_GetStudioModelInterface) (int version, struct r_studio_interface_s **ppinterface, struct engine_studio_api_s *pstudio);
//	void(*HUD_CHATINPUTPOSITION_FUNCTION) (int *x, int *y);
//	int(*HUD_GETPLAYERTEAM_FUNCTION) (int iplayer);
//	void(*CLIENTFACTORY) (void);
//} cl_clientfunc_t; 


//typedef struct cl_clientfuncs_s
//{
//	int(*Initialize) (cl_enginefunc_t* pEnginefuncs, int iVersion);
//	int(*HUD_Init) (void);
//	int(*HUD_VidInit) (void);
//	void(*HUD_Redraw) (float time, int intermission);
//	int(*HUD_UpdateClientData) (client_data_t* pcldata, float flTime);
//	int(*HUD_Reset) (void);
//	void(*HUD_PlayerMove) (struct playermove_s* ppmove, int server);
//	void(*HUD_PlayerMoveInit) (struct playermove_s* ppmove);
//	char(*HUD_PlayerMoveTexture) (char* name);
//	void(*IN_ActivateMouse) (void);
//	void(*IN_DeactivateMouse) (void);
//	void(*IN_MouseEvent) (int mstate);
//	void(*IN_ClearStates) (void);
//	void(*IN_Accumulate) (void);
//	void(*CL_CreateMove) (float frametime, struct usercmd_s* cmd, int active);
//	int(*CL_IsThirdPerson) (void);
//	void(*CL_CameraOffset) (float* ofs);
//	struct kbutton_s* (*KB_Find) (const char* name);
//	void(*CAM_Think) (void);
//	void(*V_CalcRefdef) (struct ref_params_s* pparams);
//	int(*HUD_AddEntity) (int type, struct cl_entity_s* ent, const char* modelname);
//	void(*HUD_CreateEntities) (void);
//	void(*HUD_DrawNormalTriangles) (void);
//	void(*HUD_DrawTransparentTriangles) (void);
//	void(*HUD_StudioEvent) (const struct mstudioevent_s* event, const struct cl_entity_s* entity);
//	void(*HUD_PostRunCmd) (struct local_state_s* from, struct local_state_s* to, struct usercmd_s* cmd, int runfuncs, double time, unsigned int random_seed);
//	void(*HUD_Shutdown) (void);
//	void(*HUD_TxferLocalOverrides) (struct entity_state_s* state, const struct clientdata_s* client);
//	void(*HUD_ProcessPlayerState) (struct entity_state_s* dst, const struct entity_state_s* src);
//	void(*HUD_TxferPredictionData) (struct entity_state_s* ps, const struct entity_state_s* pps, struct clientdata_s* pcd, const struct clientdata_s* ppcd, struct weapon_data_s* wd, const struct weapon_data_s* pwd);
//	void(*Demo_ReadBuffer) (int size, unsigned char* buffer);
//	int(*HUD_ConnectionlessPacket) (struct netadr_s* net_from, const char* args, char* response_buffer, int* response_buffer_size);
//	int(*HUD_GetHullBounds) (int hullnumber, float* mins, float* maxs);
//	void(*HUD_Frame) (double time);
//	int(*HUD_Key_Event) (int down, int keynum, const char* pszCurrentBinding);
//	void(*HUD_TempEntUpdate) (double frametime, double client_time, double cl_gravity, struct tempent_s** ppTempEntFree, struct tempent_s** ppTempEntActive, int(*Callback_AddVisibleEntity)(struct cl_entity_s* pEntity), void(*Callback_TempEntPlaySound)(struct tempent_s* pTemp, float damp));
//	struct cl_entity_s* (*HUD_GetUserEntity) (int index);
//	int(*HUD_VoiceStatus) (int entindex, qboolean bTalking);
//	int(*HUD_DirectorMessage) (unsigned char command, unsigned int firstObject, unsigned int secondObject, unsigned int flags);
//	int(*HUD_GetStudioModelInterface) (int version, struct r_studio_interface_s** ppinterface, struct engine_studio_api_s* pstudio);
//	void(*HUD_CHATINPUTPOSITION_FUNCTION) (int* x, int* y);
//	int(*HUD_GETPLAYERTEAM_FUNCTION) (int iplayer);
//	void(*CLIENTFACTORY) (void);
//	void(*HUD_VidSetMode) (void);
//	void(*HUD_NoticePrint) (const char* text, int color);
//	void(*CL_ClearKeyState) (void);
//	int(*ProcessCLByEngine) (int a1, unsigned int a2, int a3, int a4, int a5);
//	void(*HUD_CreateBeams) (void);
//	void(*Unk2)();
//} cl_clientfunc_t;


typedef struct cl_clientfuncs_s
{
	int(*Initialize) (cl_enginefunc_t* pEnginefuncs, int iVersion);
	int(*HUD_Init) (void);
	int(*HUD_VidInit) (void);
	void(*HUD_Redraw) (float time, int intermission);
	int(*HUD_UpdateClientData) (client_data_t* pcldata, float flTime);
	int(*HUD_Reset) (void);
	void(*HUD_PlayerMove) (struct playermove_s* ppmove, int server);
	void(*HUD_PlayerMoveInit) (struct playermove_s* ppmove);
	char(*HUD_PlayerMoveTexture) (char* name);
	void(*IN_ActivateMouse) (void);
	void(*IN_DeactivateMouse) (void);
	void(*IN_MouseEvent) (int mstate);
	void(*IN_ClearStates) (void);
	void(*IN_Accumulate) (void);
	void(*CL_CreateMove) (float frametime, struct usercmd_s* cmd, int active);
	int(*CL_IsThirdPerson) (void);
	void(*CL_CameraOffset) (float* ofs);
	struct kbutton_s* (*KB_Find) (const char* name);
	void(*CAM_Think) (void);
	void(*V_CalcRefdef) (struct ref_params_s* pparams);
	int(*HUD_AddEntity) (int type, struct cl_entity_s* ent, const char* modelname);
	void(*HUD_CreateEntities) (void);
	void(*HUD_DrawNormalTriangles) (void);
	void(*HUD_DrawTransparentTriangles) (void);
	void(*HUD_StudioEvent) (const struct mstudioevent_s* event, const struct cl_entity_s* entity);
	void(*HUD_PostRunCmd) (struct local_state_s* from, struct local_state_s* to, struct usercmd_s* cmd, int runfuncs, double time, unsigned int random_seed);
	void(*HUD_Shutdown) (void);
	void(*HUD_TxferLocalOverrides) (struct entity_state_s* state, const struct clientdata_s* client);
	void(*HUD_ProcessPlayerState) (struct entity_state_s* dst, const struct entity_state_s* src);
	void(*HUD_TxferPredictionData) (struct entity_state_s* ps, const struct entity_state_s* pps, struct clientdata_s* pcd, const struct clientdata_s* ppcd, struct weapon_data_s* wd, const struct weapon_data_s* pwd);
	void(*Demo_ReadBuffer) (int size, unsigned char* buffer);
	int(*HUD_ConnectionlessPacket) (struct netadr_s* net_from, const char* args, char* response_buffer, int* response_buffer_size);
	int(*HUD_GetHullBounds) (int hullnumber, float* mins, float* maxs);
	void(*HUD_Frame) (double time);
	int(*HUD_Key_Event) (int down, int keynum, const char* pszCurrentBinding);
	void(*HUD_TempEntUpdate) (double frametime, double client_time, double cl_gravity, struct tempent_s** ppTempEntFree, struct tempent_s** ppTempEntActive, int(*Callback_AddVisibleEntity)(struct cl_entity_s* pEntity), void(*Callback_TempEntPlaySound)(struct tempent_s* pTemp, float damp));
	struct cl_entity_s* (*HUD_GetUserEntity) (int index);
	int(*HUD_VoiceStatus) (int entindex, qboolean bTalking);
	int(*HUD_DirectorMessage) (unsigned char command, unsigned int firstObject, unsigned int secondObject, unsigned int flags);
	int(*HUD_GetStudioModelInterface) (int version, struct r_studio_interface_s** ppinterface, struct engine_studio_api_s* pstudio);
	void(*HUD_CHATINPUTPOSITION_FUNCTION) (int* x, int* y);
	int(*HUD_GETPLAYERTEAM_FUNCTION) (int iplayer);
	void(*CLIENTFACTORY) (void);
	void(*HUD_VidSetMode) (void);
	void(*HUD_NoticePrint) (const char* text, int color);
	void(*CL_ClearKeyState) (void);
	void(*ProcessCLByEngine) (int a1, unsigned int a2, int a3, int a4, int a5);
	void(*HUD_CreateBeams) (void);
} cl_clientfunc_t;

extern cl_clientfunc_t *g_pClient;
extern cl_enginefunc_t *g_pEngine;
extern engine_studio_api_t *g_pStudio;
extern r_studio_interface_t* g_pInterface;
extern StudioModelRenderer_t* g_pStudioModelRenderer;
extern playermove_t* pmove;
typedef void(*CL_Move_t)();

extern CL_Move_t CL_Move_s;
extern PreS_DynamicSound_t PreS_DynamicSound_s;
extern cl_clientfunc_t g_Client;
extern cl_enginefunc_t g_Engine;
extern engine_studio_api_t g_Studio;
extern r_studio_interface_t g_Interface;
extern StudioModelRenderer_t g_StudioModelRenderer;
extern PColor24 Console_TextColor;

extern DWORD HudRedraw;

#endif