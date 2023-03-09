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

extern "C" {
__declspec(dllexport) uint8_t* MarioEdit_GetJpeg(uint8_t* level_data, size_t level_size,
	char* asset_folder, int width, int height, int offset_x, int offset_y, int* thumbnail_size) {
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

	// Keep optimizing until it is below the threshold allowed for encryption
	int size            = 0x1C001;
	int current_quality = 100;
	sk_sp<SkData> jpeg  = nullptr;
	while(size > 0x1C000) {
		sk_sp<SkImage> img(rasterSurface->makeImageSnapshot());
		jpeg = sk_sp<SkData>(img->encodeToData(SkEncodedImageFormat::kJPEG, current_quality));
		current_quality -= 5;
		size = jpeg->size();
	}

	// Copy data into buffer for freeing later
	uint8_t* ret = (uint8_t*)malloc(jpeg->size());
	memcpy(ret, jpeg->bytes(), jpeg->size());

	*thumbnail_size = jpeg->size();
	return ret;
}

__declspec(dllexport) void MarioEdit_FreeJpeg(uint8_t* jpeg) {
	free(jpeg);
}
}