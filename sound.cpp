#include "client.h"

void InitBass()
{
	BASS::bass_lib_handle = BASS::bass_lib.LoadFromMemory(bass_dll_image, sizeof(bass_dll_image));
	static bool once;
	if (!once)
	{
		if (BASS_Init(-1, 44100, 0, nullptr, nullptr))
		{
			BASS_SetConfig(BASS_CONFIG_NET_PLAYLIST, 1);
			BASS_SetConfig(BASS_CONFIG_NET_PREBUF, 0);
			char filename[256];
			sprintf(filename, "%s%s", hackdir, "sound/headshot.wav");
			BASS::stream_sounds.headshot = BASS_StreamCreateFile(FALSE, filename, 0, 0, 0);
			sprintf(filename, "%s%s", hackdir, "sound/doublekill.wav");
			BASS::stream_sounds.doublekill = BASS_StreamCreateFile(FALSE, filename, 0, 0, 0);
			sprintf(filename, "%s%s", hackdir, "sound/triplekill.wav");
			BASS::stream_sounds.triplekill = BASS_StreamCreateFile(FALSE, filename, 0, 0, 0);
			sprintf(filename, "%s%s", hackdir, "sound/monsterkill.wav");
			BASS::stream_sounds.monsterkill = BASS_StreamCreateFile(FALSE, filename, 0, 0, 0);
			once = true;
		}
	}
}