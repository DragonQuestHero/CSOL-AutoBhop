#include "client.h"

int StudioCheckBBox(void)
{
	if ((cvar.rage_active && cvar.rage_fov > 45) ||
		(cvar.legit[g_Local.weapon.m_iWeaponID].active && cvar.legit[g_Local.weapon.m_iWeaponID].fov > 45) ||
		(cvar.knifebot_active && cvar.knifebot_fov > 45))
		return 1;
	return g_Studio.StudioCheckBBox();
}

void StudioSetRemapColors(int top, int bottom)
{
	cl_entity_s* ent = g_Studio.GetCurrentEntity();
	GetHitboxes(ent);
	SetSkins(ent);
	g_Studio.StudioSetRemapColors(top, bottom);
}

typedef struct
{
	mstudiomesh_t pmesh;
	int meshindex;
	int numbodyparts;
	int nummodels;
	int nummesh;
	char modelname[MAX_MODEL_NAME];
} viewmodel_t;

deque<viewmodel_t> ViewModel;

void StudioDrawPoints()
{
	cl_entity_t* ent = g_Engine.GetViewModel();
	if (!ent)
		return g_Studio.StudioDrawPoints();
	studiohdr_t* pStudioHeader = (studiohdr_t*)g_Studio.Mod_Extradata(ent->model);
	if (!pStudioHeader)
		return g_Studio.StudioDrawPoints();
	mstudiobodyparts_t* pBodyPart = (mstudiobodyparts_t*)((byte*)pStudioHeader + pStudioHeader->bodypartindex);
	if (!pBodyPart)
		return g_Studio.StudioDrawPoints();

	if (cvar.visual_skins_viewmodel_nohands && DrawVisuals && (!cvar.route_auto || cvar.route_draw_visual) && GetTickCount() - HudRedraw <= 100)
	{
		bool savedmodel = false;
		for (viewmodel_t meshes : ViewModel)
		{
			if (strstr(meshes.modelname, ent->model->name))
			{
				savedmodel = true;
				break;
			}
		}
		if (!savedmodel)
		{
			for (int k = 0; k < pStudioHeader->numbodyparts; k++)
			{
				mstudiomodel_t* pSubModel = (mstudiomodel_t*)((byte*)pStudioHeader + pBodyPart[k].modelindex);
				for (int i = 0; i < pBodyPart[k].nummodels; i++)
				{
					short* pskinref = (short*)((byte*)pStudioHeader + pStudioHeader->skinindex);;
					mstudiotexture_t* ptexture = (mstudiotexture_t*)((byte*)pStudioHeader + pStudioHeader->textureindex);;
					mstudiomesh_t* pmesh = (mstudiomesh_t*)((byte*)pStudioHeader + pSubModel[i].meshindex);

					int meshindexes = 0;
					for (int j = 0; j < pSubModel[i].nummesh; j++)
					{
						if (strstr(ptexture[pskinref[pmesh[j].skinref]].name, "view_glove.") ||
							strstr(ptexture[pskinref[pmesh[j].skinref]].name, "view_finger.") ||
							strstr(ptexture[pskinref[pmesh[j].skinref]].name, "view_skin."))
						{
							bool savedskin = false;
							for (viewmodel_t meshes : ViewModel)
							{
								if (strstr(meshes.modelname, ent->model->name) && meshes.meshindex == j - meshindexes && meshes.numbodyparts == k && meshes.nummodels == i)
								{
									savedskin = true;
									break;
								}
							}
							if (!savedskin)
							{
								viewmodel_t meshes;
								meshes.meshindex = j - meshindexes;
								meshes.pmesh = pmesh[j - meshindexes];
								meshes.numbodyparts = k;
								meshes.nummodels = i;
								meshes.nummesh = pSubModel[i].nummesh;
								sprintf(meshes.modelname, ent->model->name);
								ViewModel.push_back(meshes);
							}
							pmesh[j - meshindexes] = pmesh[j + 1];
							meshindexes++;
						}
					}
					pSubModel[i].nummesh -= meshindexes;
				}
			}
		}
	}
	else
	{
		for (unsigned int x = 0; x < ViewModel.size(); x++)
		{
			if (strstr(ViewModel[x].modelname, ent->model->name))
			{
				mstudiomodel_t* pSubModel = (mstudiomodel_t*)((byte*)pStudioHeader + pBodyPart[ViewModel[x].numbodyparts].modelindex);
				mstudiomesh_t* pmesh = (mstudiomesh_t*)((byte*)pStudioHeader + pSubModel[ViewModel[x].nummodels].meshindex);

				pmesh[ViewModel[x].meshindex] = ViewModel[x].pmesh; 
				pSubModel[ViewModel[x].nummodels].nummesh = ViewModel[x].nummesh;
				ViewModel.erase(ViewModel.begin() + x);
			}
		}
	}
	g_Studio.StudioDrawPoints();
}

void HookStudioFunctions()
{
	g_pStudio->StudioDrawPoints = StudioDrawPoints;
	g_pStudio->StudioSetRemapColors = StudioSetRemapColors;
	g_pStudio->StudioCheckBBox = StudioCheckBBox;
}
