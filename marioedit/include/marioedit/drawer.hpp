#pragma once

#include <marioedit/parser.hpp>

#include <array>
#include <codec/SkCodec.h>
#include <core/SkBitmap.h>
#include <core/SkCanvas.h>
#include <core/SkColor.h>
#include <core/SkFont.h>
#include <core/SkImage.h>
#include <core/SkStream.h>
#include <core/SkSurface.h>
#include <core/SkTextBlob.h>
#include <core/SkTypeface.h>
#include <cstdint>
#include <encode/SkPngEncoder.h>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace MarioEdit {
	namespace Level {
		class Drawer {
		public:
			using Point = Parser::Point;

			Drawer(Parser& level_, int zoom)
				: level(level_) {
				Zm           = zoom;
				H            = level.MapHdr.BorT / 16;
				W            = level.MapHdr.BorR / 16;
				renderWidth  = (level.MapHdr.BorR / 16) * Zm;
				renderHeight = (level.MapHdr.BorT / 16) * Zm;
			};

			void Setup();
			void SetGraphics(SkCanvas* canvas);
			sk_sp<SkImage> ImageFromPath(std::string path);
			void LoadTilesheet();
			void SetIsOverworld(bool isOverworld);
			void SetLog(bool log);
			void SetAssetFolder(std::string folder);
			void SetOffsetX(int x);
			void SetOffsetY(int y);
			int GetWidth();
			int GetHeight();
			void ClearImageCache();
			void DrawGridlines();
			sk_sp<SkImage> GetCachedTile(int x, int y, int w, int h);
			sk_sp<SkImage> GetCachedPattern(int id);
			void DrawTile(int tileX, int tileY, int tileW, int tileH, int x, int y, int targetWidth,
				int targetHeight);
			void DrawImage(uint32_t id, int x, int y, int targetWidth, int targetHeight);
			void DrawImageOpacity(
				uint32_t id, double opacity, int x, int y, int targetWidth, int targetHeight);
			void DrawImageRotate(
				uint32_t id, double angle, int x, int y, int targetWidth, int targetHeight);
			void DrawImageRotateOpacity(uint32_t id, double angle, double opacity, int x, int y,
				int targetWidth, int targetHeight);
			void DrawCrp(unsigned char EX, int X, int Y);
			void DrawSnake(unsigned char EX, int X, int Y, int SW, int SH);
			void DrawMoveBlock(unsigned char ID, unsigned char EX, int X, int Y);
			void DrawItem(const std::unordered_set<short>& K, bool L);
			void ReGrdCode();
			void DrawGrd();
			int GetGrdType(const std::string& A);
			std::string GetGrdCode(int x, int y);
			int GetGrdBold(int x, int y);
			Point GetCorCode(int x, int y);
			int CalC(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4);
			void DrawGrdCode();
			void DrawIce();
			void DrawTrack();
			void DrawCID();
			void DrawFireBar();
			void DrawFire();
			void DrawCPipe();

		private:
			int TileW;
			int H            = 0;
			int W            = 0;
			int renderWidth  = 0;
			int renderHeight = 0;
			int Zm           = 16;
			int NowIO        = 0;
			int offsetX      = 0;
			int offsetY      = 0;

			SkCanvas* c = NULL;
			Parser& level;
			bool doLogging = false;

			std::string assetFolder;
			std::string tilesheetPath;

			std::unordered_map<int, sk_sp<SkImage>> tileCache;
			std::unordered_map<int, sk_sp<SkImage>> patternCache;
			sk_sp<SkImage> tilesheet   = NULL;
			sk_sp<SkImage> spritesheet = NULL;
		};
	}
}