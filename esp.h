#ifndef _ESP_
#define _ESP_

typedef struct
{
	Vector HitboxMulti[8];
} playeresphitbox_t;

typedef struct
{
	cl_entity_s* ent;
	deque<playeresphitbox_t> PlayerEspHitbox;
} playeresp_t;
extern deque<playeresp_t> PlayerEsp;

void DrawPlayerEsp();
void DrawPlayerSoundIndexEsp();
void DrawPlayerSoundNoIndexEsp();

#endif