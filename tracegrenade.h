#ifndef _TRACEGRENADE_
#define _TRACEGRENADE_

typedef struct
{
	Vector GrenadePos1;
	Vector GrenadePos2;
} grenadeline_t;

extern deque<grenadeline_t> Grenadeline;

void TraceGrenade(ref_params_s* pparams);
void DrawTraceGrenade();

#endif