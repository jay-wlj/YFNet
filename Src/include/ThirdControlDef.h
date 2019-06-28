#pragma once

enum Third_cmd
{
	Third_cmd_resume = 1,
	Third_cmd_pause,
	Third_cmd_stop,
	Third_cmd_volume_up,
	Third_cmd_volume_down,
	Third_cmd_mute,
	Third_cmd_unmute,
	Third_cmd_next,
	Third_cmd_prev,
	Third_cmd_forward,
	Third_cmd_backward,
	Third_cmd_activate,
};
enum Third_CopydataID
{
	Third_Copydata_SearchAndPlay = 1,
	Third_Copydata_PlayLocalFile,
};
struct Third_LocalFile
{
	char szDirPath[MAX_PATH];
	char szMediaName[MAX_PATH];
};