#pragma once

#if defined(_MSC_VER)
#define MARIOEDIT_API __declspec(dllexport)                  // Microsoft
#elif defined(__GNUC__)
#define MARIOEDIT_API __attribute__((visibility("default"))) // GCC
#else
#define MARIOEDIT_API // Most compilers export all the symbols by default
#pragma warning Unknown dynamic link import / export semantics.
#endif

#include <stdint.h>
#include <stdlib.h>

MARIOEDIT_API uint8_t* MarioEdit_GetJpeg(uint8_t* level_data, size_t level_size, char* asset_folder,
	int width, int height, int offset_x, int offset_y, int* thumbnail_size);
MARIOEDIT_API uint8_t* MarioEdit_GetFullPng(uint8_t* level_data, size_t level_size,
	char* asset_folder, int width, int height, int offset_x, int offset_y, int* thumbnail_size);
MARIOEDIT_API void MarioEdit_FreeImage(uint8_t* image);