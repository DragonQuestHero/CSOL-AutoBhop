#include "client.h"

bool loadtexturesky = true;

void LoadTextureImageSky(char* image, int index)
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
}

void Sky()
{
	static int index[6];

	if (cvar.visual_sky && DrawVisuals && (!cvar.route_auto || cvar.route_draw_visual) && GetTickCount() - HudRedraw <= 100)
	{
		if (loadtexturesky)
		{
			LoadTextureImageSky("texture/sky/rt.bmp", 20);
			LoadTextureImageSky("texture/sky/bk.bmp", 21);
			LoadTextureImageSky("texture/sky/lf.bmp", 22);
			LoadTextureImageSky("texture/sky/ft.bmp", 23);
			LoadTextureImageSky("texture/sky/up.bmp", 24);
			LoadTextureImageSky("texture/sky/dn.bmp", 25);
		}
		if (!index[0] && !index[1] && !index[2] && !index[3] && !index[4] && !index[5] || loadtexturesky)
		{
			int* gSkyTexNumber = c_Offset.FindSkyTexNumber();
			for (unsigned int i = 0; i < 6; i++)
			{
				if (!index[i])
				{
					index[i] = gSkyTexNumber[i];
					gSkyTexNumber[i] = texture_id[i + 20];
				}
				else if (loadtexturesky)
					gSkyTexNumber[i] = texture_id[i + 20];
			}
		}
		loadtexturesky = false;
	}
	else
	{
		if (index[0] && index[1] && index[2] && index[3] && index[4] && index[5])
		{
			int* gSkyTexNumber = c_Offset.FindSkyTexNumber();
			for (unsigned int i = 0; i < 6; i++)
			{
				gSkyTexNumber[i] = index[i];
				index[i] = 0;
			}
		}
	}
}