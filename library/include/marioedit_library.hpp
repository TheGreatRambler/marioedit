#pragma once

#include <stdint.h>

__declspec(dllexport) uint8_t* MarioEdit_GetJpeg(uint8_t* level_data, size_t level_size,
	char* asset_folder, int width, int height, int offset_x, int offset_y, int* thumbnail_size);
__declspec(dllexport) void MarioEdit_FreeJpeg(uint8_t* jpeg);