#ifndef _SKELETON_
#define _SKELETON_

typedef struct
{
	int parent;
	Vector vBone;
	Vector vBoneParent;
} worldbone_t;
extern deque<worldbone_t> WorldBone;

typedef struct
{
	Vector vCubePointsTrans[8];
} worldhitbox_t;
extern deque<worldhitbox_t> WorldHitbox;

typedef struct
{
	int index; 
	int parent;
	Vector vBone;
	Vector vBoneParent;
} playerbone_t;
extern deque<playerbone_t> PlayerBone;

typedef struct
{
	int index;
	Vector vCubePointsTrans[8];
} playerhitbox_t;
extern deque<playerhitbox_t> PlayerHitbox;

typedef struct
{
	int Hitbox;
	Vector HitboxPos;
} playerhitboxnum_t;
extern deque<playerhitboxnum_t> PlayerHitboxNum;

extern int HeadBox[33];
void GetHitboxes(cl_entity_s* ent);
void DrawSkeletonPlayer();
void DrawSkeletonWorld();
void VectorTransform(Vector in1, float in2[3][4], float* out);
bool IsSHield(Vector origin[8]);

#endif
