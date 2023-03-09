#include <chrono>
#include <codec/SkCodec.h>
#include <core/SkBitmap.h>
#include <core/SkCanvas.h>
#include <core/SkColor.h>
#include <core/SkFont.h>
#include <core/SkGraphics.h>
#include <core/SkImage.h>
#include <core/SkStream.h>
#include <core/SkSurface.h>
#include <core/SkTextBlob.h>
#include <core/SkTypeface.h>
#include <encode/SkPngEncoder.h>
#include <fstream>
#include <marioedit.hpp>
#include <ostream>
#include <thread>

uint8_t* MarioEdit_GetJpeg(uint8_t* level_data, size_t level_size, char* asset_folder, int width,
	int height, int offset_x, int offset_y) {
	sk_sp<SkSurface> rasterSurface = SkSurface::MakeRasterN32Premul(width, height);

	MarioEdit::Level::Parser* levelParser = new MarioEdit::Level::Parser();
	levelParser->LoadLevelData(std::string((char*)level_data, level_size), true);

	MarioEdit::Level::Drawer* drawer = new MarioEdit::Level::Drawer(*levelParser, 16);
	drawer->Setup();
	drawer->SetIsOverworld(true);
	drawer->SetLog(false);
	drawer->SetAssetFolder(asset_folder);
	drawer->SetGraphics(rasterSurface->getCanvas());
	drawer->LoadTilesheet();

	drawer->SetOffsetX(offset_x);
	drawer->SetOffsetY(offset_y);

	rasterSurface->getCanvas()->clear(SK_ColorBLACK);
	MarioEdit::Viewer::DrawMap(drawer);
	rasterSurface->getCanvas()->flush();

	sk_sp<SkImage> img(rasterSurface->makeImageSnapshot());
	sk_sp<SkData> jpeg(img->encodeToData(SkEncodedImageFormat::kJPEG, 95));

	// Copy data into buffer for freeing later
	uint8_t* ret = (uint8_t*)malloc(jpeg->size());
	memcpy(ret, jpeg->bytes(), jpeg->size());

	return ret;
}

void MarioEdit_FreeJpeg(uint8_t* jpeg) {
	free(jpeg);
}