#ifndef _AIMBOT_
#define _AIMBOT_

typedef struct
{
	float r;
	float g;
	float b;
	float a;
	int Radius;
	ImVec2 Pos;
} fov_t;
extern deque<fov_t> FOVDraw;

typedef struct
{
	float HitboxFOV;
	Vector Hitbox;
	float HitboxPointsFOV[8];
	Vector HitboxMulti[8];
} playeraimhitbox_t;

typedef struct
{
	char modelname[MAX_MODEL_NAME];
	deque<playeraimhitbox_t> PlayerAimHitbox;
	cl_entity_s* ent;
} playeraim_t;
extern deque<playeraim_t> PlayerAim;

typedef struct
{
	int numhitbox;
	char displaymodel[MAX_QPATH];
	char checkmodel[MAX_QPATH]; 
	int m_iWeaponID;
} playeraimlegit_t;
extern deque<playeraimlegit_t> PlayerAimLegit;

static BYTE SkeletonHitboxMatrix[12][2] = { {0,1},{1,2},{2,3},{2,4},{4,5},{5,6},{6,7},{1,5},{0,6},{3,7},{7,4},{0,3}, };
extern DWORD dwReactionTime;
extern bool TriggerKeyStatus;
extern bool LegitKeyStatus;
extern bool RageKeyStatus;

extern int iTargetTrigger;
extern int iTargetLegit;
extern int iTargetRage;
extern int iTargetKnife;

void AimBot(struct usercmd_s* cmd);
void DrawAimBotFOV();
void ContinueFire(usercmd_s* cmd);
void DrawAimbot();
void SmoothAimAngles(QAngle MyViewAngles, QAngle AimAngles, QAngle& OutAngles, float Smoothing, bool bSpiral, float SpiralX, float SpiralY);
int FireBullets(Vector start, Vector end, float flDistance, int iOriginalPenetration, int iBulletType, int iDamage, float flRangeModifier);
bool IsBoxIntersectingRay(const Vector& boxMin, const Vector& boxMax, const Vector& origin, const Vector& delta);

#endif