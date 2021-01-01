#include "client.h"

bool selected[1024];
bool newimage;
bool resetone[1024];
bool resetall;
int arraynum;
int resetindex[1024];
bool loadtexturewall = true;
char imagenamenew[1024][256];
int imagewidthnew[1024];
int imageheightnew[1024];
int checkdrawrestoreindex[1024];
bool loadwall;
bool savewall;
int saveindex[1024];
bool autoloadwall;
bool menurunning;
bool gamerunning = false;

void Savefile()
{
	cl_entity_s* ent = g_Engine.GetEntityByIndex(0);
	if (!ent || !ent->model)return;
	char filename[256], levelname[256];
	char tfilename[256];
	strcpy(levelname, g_Engine.pfnGetLevelName() + 5);
	int len = strlen(levelname);
	if (len > 4)levelname[len - 4] = (char)0;
	sprintf(tfilename, "texture/wallsave/%s.wall", levelname);
	sprintf(filename, "%s%s", hackdir, tfilename);
	remove(filename);
	ofstream ofs(filename, ios::binary | ios::app);
	for (unsigned int i = 0; i < ent->model->numtextures; i++)
	{
		selected[i] = false;
		if (saveindex[i])
		{
			char text[256];
			sprintf(text, "wallsave(%d)(%d)", i, saveindex[i]);
			ofs << text << (char)0x0D << (char)0x0A;
		}
	}
	ofs.close();
}

void Loadfile()
{
	cl_entity_s* ent = g_Engine.GetEntityByIndex(0);
	if (!ent || !ent->model)return;
	char filename[256], levelname[256];
	char tfilename[256];
	strcpy(levelname, g_Engine.pfnGetLevelName() + 5);
	int len = strlen(levelname);
	if (len > 4)levelname[len - 4] = (char)0;
	sprintf(tfilename, "texture/wallsave/%s.wall", levelname);
	sprintf(filename, "%s%s", hackdir, tfilename);
	ifstream ifs(filename);
	while (ifs.good())
	{
		char buf[1024];
		ifs.getline(buf, sizeof(buf));
		int index = - 1, imageindex = -1;
		if (sscanf(buf, "wallsave(%d)(%d)", &index, &imageindex))
		{
			if (index != -1 && imageindex != -1)
			{
				if(!resetindex[index])
					resetindex[index] = ent->model->textures[index]->gl_texturenum;
				if (texture_id[imageindex])
				{
					ent->model->textures[index]->gl_texturenum = texture_id[imageindex];
					saveindex[index] = imageindex;
					selected[index] = false;
				}
			}
		}
	}
	ifs.close();
}

void AutoLoadfile()
{
	cl_entity_s* ent = g_Engine.GetEntityByIndex(0);
	if (!ent || !ent->model)return;
	for (unsigned int i = 0; i < 1024; i++)
	{
		selected[i] = false;
		checkdrawrestoreindex[i] = 0;
		resetindex[i] = 0;
		saveindex[i] = 0;
	}
	char filename[256], levelname[256];
	char tfilename[256];
	strcpy(levelname, g_Engine.pfnGetLevelName() + 5);
	int len = strlen(levelname);
	if (len > 4)levelname[len - 4] = (char)0;
	sprintf(tfilename, "texture/wallsave/%s.wall", levelname);
	sprintf(filename, "%s%s", hackdir, tfilename);
	ifstream ifs(filename);
	while (ifs.good())
	{
		char buf[1024];
		ifs.getline(buf, sizeof(buf));
		int index = -1, imageindex = -1;
		if (sscanf(buf, "wallsave(%d)(%d)", &index, &imageindex))
		{
			if (index != -1 && imageindex != -1)
			{
				if (texture_id[imageindex])
				{
					resetindex[index] = ent->model->textures[index]->gl_texturenum;
					ent->model->textures[index]->gl_texturenum = texture_id[imageindex];
					saveindex[index] = imageindex;
				}
			}
		}
	}
	ifs.close();
}

void LoadTextureImageWall(char* image, int index)
{
	char filename[256];
	int width, height;
	sprintf(filename, "%s%s", hackdir, image);
	GLint last_texture;
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
	if (texture_id[index])glDeleteTextures(1, &texture_id[index]);
	GLuint glindex;
	glGenTextures(1, &glindex);
	texture_id[index] = glindex + 20000;
	glBindTexture(GL_TEXTURE_2D, texture_id[index]);
	unsigned char* soilimage = SOIL_load_image(filename, &width, &height, 0, SOIL_LOAD_RGBA);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, soilimage);
	SOIL_free_image_data(soilimage);
	glBindTexture(GL_TEXTURE_2D, last_texture);

	imagewidthnew[index] = width;
	imageheightnew[index] = height;
}

void GetTextureWall()
{
	for (unsigned int i = 0; i < 1024; i++)
	{
		imagenamenew[i][0] = (char)0;
		imagewidthnew[i] = 0;
		imageheightnew[i] = 0;
	}

	char filedir[256];
	sprintf(filedir, "%s%s", hackdir, "texture/wall\\*");
	WIN32_FIND_DATA fd;
	HANDLE hFind = ::FindFirstFile(filedir, &fd);
	int index = 501;
	if (hFind != INVALID_HANDLE_VALUE)
	{
		do
		{
			if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				if (index < 1024)
				{
					char filename[256];
					sprintf(filename, "texture/wall/%s", fd.cFileName);
					char name[256];
					sprintf(name, "%s", fd.cFileName);
					int len = strlen(name);
					if (len > 4)name[len - 4] = (char)0;
					sprintf(imagenamenew[index], "%s", name);
					LoadTextureImageWall(filename, index);
					index++;
				}
			}
		} while (::FindNextFile(hFind, &fd));
		::FindClose(hFind);
	}
}

void LoadTextureWall()
{
	cl_entity_s* ent = g_Engine.GetEntityByIndex(0);
	static bool checkgame = false;
	if (ent && ent->model && g_Engine.GetMaxClients())
		gamerunning = true;
	else
		gamerunning = false;

	if (checkgame != gamerunning)
	{
		if (gamerunning)
			autoloadwall = true;
		checkgame = gamerunning;
	}
}

void WallRun()
{
	if(loadtexturewall) 
		GetTextureWall(), loadtexturewall = false;

	if (menurunning != bShowMenu)
	{
		if (!bShowMenu)
		{
			for (unsigned int i = 0; i < 1024; i++)
				selected[i] = false;
		}
		menurunning = bShowMenu;
	}

	if (loadwall)
		Loadfile(), loadwall = false;

	if (savewall)
		Savefile(), savewall = false;

	if (autoloadwall && cvar.visual_skins_wall)
		AutoLoadfile(), autoloadwall = false;

	cl_entity_s* ent = g_Engine.GetEntityByIndex(0);
	if (!ent || !ent->model || !gamerunning)
		return;

	if (cvar.visual_skins_wall && DrawVisuals && (!cvar.route_auto || cvar.route_draw_visual) && GetTickCount() - HudRedraw <= 100)
	{
		for (unsigned int i = 0; i < ent->model->numtextures; i++)
		{
			if (ent->model->textures[i]->name && ent->model->textures[i]->gl_texturenum)
			{
				if ((resetall || resetone[i]))
				{
					saveindex[i] = 0;
					if (resetindex[i])
						ent->model->textures[i]->gl_texturenum = resetindex[i];
					if (resetone[i])
					{
						if (selected[i])
							selected[i] = false;
						else
							selected[i] = true;
					}
					if (resetall && selected[i]) selected[i] = false;
					resetindex[i] = 0;
					resetone[i] = false;
				}
				if (selected[i] && newimage)
				{
					if (!resetindex[i])
						resetindex[i] = ent->model->textures[i]->gl_texturenum;
					ent->model->textures[i]->gl_texturenum = texture_id[arraynum];
					saveindex[i] = arraynum;
					selected[i] = false;
				}
				if (checkdrawrestoreindex[i])
				{	
					ent->model->textures[i]->gl_texturenum = checkdrawrestoreindex[i];
					checkdrawrestoreindex[i] = 0;
				}
			}
		}
		newimage = false;
		resetall = false;
	}
	else
	{
		for (unsigned int i = 0; i < ent->model->numtextures; i++)
		{
			if (ent->model->textures[i]->name && ent->model->textures[i]->gl_texturenum)
			{
				if (resetindex[i] && !checkdrawrestoreindex[i])
				{
					checkdrawrestoreindex[i] = ent->model->textures[i]->gl_texturenum;
					ent->model->textures[i]->gl_texturenum = resetindex[i];
				}
			}
		}
	}
}