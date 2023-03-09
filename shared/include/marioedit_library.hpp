#pragma once

#include <stdint.h>

uint8_t* MarioEdit_GetJpeg(uint8_t* level_data, size_t level_size, char* asset_folder, int width,
	int height, int offset_x, int offset_y);
void MarioEdit_FreeJpeg(uint8_t* jpeg);