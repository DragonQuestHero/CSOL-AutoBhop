#include "client.h"

void SaveHitbox()
{
	char filename[256];
	sprintf(filename, "%s%s", hackdir, "model/model.hitbox");
	remove(filename);
	ofstream ofs(filename, ios::binary | ios::app);
	for (model_aim_select_t Model_Selected : Model_Aim_Select)
	{
		char text[256];
		sprintf(text, "Display: %s Path: %s Hitbox: %d", Model_Selected.displaymodel, Model_Selected.checkmodel, Model_Selected.numhitbox);
		ofs << text << (char)0x0D << (char)0x0A;
	}
	ofs.close();
}

void LoadHitbox()
{
	char filename[256];
	sprintf(filename, "%s%s", hackdir, "model/model.hitbox");
	ifstream ifs(filename);
	while (ifs.good())
	{
		char buf[1024];
		ifs.getline(buf, sizeof(buf));
		int hitbox = -1;
		char display[256];
		char path[256];
		if (sscanf(buf, "%*s %s %*s %s %*s %d", &display, &path, &hitbox))
		{
			if (display && path && hitbox != -1)
			{
				bool saved = false;
				for (model_aim_select_t Model_Selected : Model_Aim_Select)
				{
					if (strstr(Model_Selected.checkmodel, path) && Model_Selected.numhitbox == hitbox)
						saved = true;
				}
				if (!saved)
				{
					model_aim_select_t Model_Select;
					sprintf(Model_Select.displaymodel, display);
					sprintf(Model_Select.checkmodel, path);
					Model_Select.numhitbox = hitbox;
					Model_Aim_Select.push_back(Model_Select);
				}
			}
		}
	}
	ifs.close();
}