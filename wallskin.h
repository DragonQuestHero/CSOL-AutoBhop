#ifndef _WALLSKIN_
#define _WALLSKIN_

extern bool selected[1024];
extern int resetindex[1024];
extern bool resetall;
extern int arraynum;
extern bool newimage;
extern bool resetone[1024];
extern bool loadtexturewall;
extern char imagenameengine[1024][256];
extern char imagenamenew[1024][256];
extern int imagewidthnew[1024];
extern int imageheightnew[1024];
extern bool autoloadwall;
extern bool loadwall;
extern bool savewall;
extern bool loadfile;

void WallRun(); 
void LoadTextureWall();

#endif