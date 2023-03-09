#pragma once

#include <stdint.h>

namespace MarioEdit {
	uint8_t* GetJpeg(uint8_t* level_data, size_t level_size, char* asset_folder, int width,
		int height, int offset_x, int offset_y);
	void FreeJpeg(uint8_t* jpeg);
}