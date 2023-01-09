#define SK_GL 1

#include <marioedit.hpp>
#include <marioedit/level.hpp>

#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>
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
#include <filesystem>
#include <fmt/format.h>
#include <fstream>
#include <gpu/GrBackendSurface.h>
#include <gpu/GrDirectContext.h>
#include <gpu/gl/GrGLInterface.h>
#include <thread>

void DrawMap(LevelDrawer* drawer) {
	drawer->DrawGridlines();

	drawer->DrawItem({ 132 }, false);
	drawer->DrawItem({ 16 }, false);
	drawer->DrawItem({ 14 }, false);
	drawer->DrawItem({ 17 }, false);
	drawer->DrawItem({ 113 }, false);
	drawer->DrawItem({ 71 }, false);

	drawer->DrawItem({ 66, 67, 106 }, false);
	drawer->DrawItem({ 64 }, false);
	drawer->DrawItem({ 90 }, false);

	drawer->DrawItem({ 106, 107 }, false);

	drawer->ReGrdCode();
	drawer->DrawGrd();
	// DrawSlope()
	drawer->DrawGrdCode();

	drawer->DrawItem({ 53, 94, 99, 100, 79 }, false);
	drawer->DrawIce();

	drawer->DrawItem({ 9, 55, 84, 97 }, false);
	drawer->DrawItem({ 85, 119 }, false);
	drawer->DrawItem({ 105 }, false);
	drawer->DrawTrack();
	drawer->DrawItem({ 4, 5, 6, 21, 22, 23, 29, 43, 63, 110, 108 }, false);

	drawer->DrawItem({ 91, 36, 11 }, false);

	drawer->DrawItem({ 83 }, false);

	drawer->DrawItem({ 68, 82 }, false);

	drawer->DrawItem(
		{ 0, 1, 2, 3, 8, 10, 12, 13, 15, 18, 19, 20, 25, 28, 30, 31, 32, 33, 34, 35, 39 }, false);
	drawer->DrawItem({ 40, 41, 42, 44, 45, 46, 47, 48, 52, 56, 57, 58, 60, 61, 62, 70, 74, 76, 77,
						 78, 81, 92, 95, 98, 102, 103, 104 },
		false);
	drawer->DrawItem({ 111, 120, 121, 122, 123, 124, 125, 126, 112, 127, 128, 129, 130, 131, 72, 50,
						 51, 65, 80, 114, 116 },
		false);
	drawer->DrawItem({ 96, 117, 86 }, false);
	drawer->DrawItem({ 24, 54 }, false);

	// DrawFireBar(False)
	// DrawFire(False)
	drawer->DrawItem({ 105 }, false);
	drawer->DrawTrack();
	drawer->DrawItem({ 105 }, true);
	// DrawItem("/89/", False)

	drawer->DrawItem({ 4, 5, 6, 21, 22, 23, 29, 43, 63 }, true);

	drawer->DrawItem({ 91, 36, 11 }, true);

	drawer->DrawItem({ 68, 82 }, true);

	drawer->DrawItem(
		{ 0, 1, 2, 3, 8, 10, 12, 13, 15, 18, 19, 20, 25, 28, 30, 31, 32, 33, 34, 35, 39 }, true);
	drawer->DrawItem({ 40, 41, 42, 44, 45, 46, 47, 48, 52, 56, 57, 58, 60, 61, 62, 70, 74, 76, 77,
						 78, 81, 92, 95, 98, 102, 103, 104 },
		true);
	drawer->DrawItem({ 111, 120, 121, 122, 123, 124, 125, 126, 112, 127, 128, 129, 130, 131, 72, 50,
						 51, 65, 80, 114, 116 },
		true);
	drawer->DrawItem({ 96, 117, 86 }, true);

	drawer->DrawCID();

	drawer->DrawItem({ 24, 54 }, true);
	drawer->DrawFireBar();
	drawer->DrawFire();

	drawer->DrawCPipe();

	if(true) {
		drawer->DrawItem({ 9, 42 }, true);
	}
}

int MarioEdit::Viewer::Start() {
	SDL_Window* window       = nullptr;
	SDL_GLContext gl_context = nullptr;

	if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMEPAD) != 0) {
		printf("Error: %s\n", SDL_GetError());
		return -1;
	}

	// Decide GL+GLSL versions
#if defined(__EMSCRIPTEN__)
	// WebGL1 + GL ES2
	const char* glsl_version = "#version 100";
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#elif defined(IMGUI_IMPL_OPENGL_ES2)
	// GL ES 2.0 + GLSL 100
	const char* glsl_version = "#version 100";
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#elif defined(__APPLE__)
	// GL 3.2 Core + GLSL 150
	const char* glsl_version = "#version 150";
	SDL_GL_SetAttribute(
		SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG); // Always required on Mac
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
#else
	// GL 3.0 + GLSL 130
	const char* glsl_version = "#version 130";
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#endif

	static const int kStencilBits = 8; // Skia needs 8 stencil bits
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 0);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, kStencilBits);

	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

	SDL_DisplayMode current;
	SDL_GetCurrentDisplayMode(0, &current);
	int w = current.w;
	int h = current.h - 65;

	SDL_WindowFlags window_flags
		= (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI
							| SDL_WINDOW_MAXIMIZED);
	window     = SDL_CreateWindow("MarioEdit | Super Mario Maker 2 Level Editor",
			SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h, window_flags);
	gl_context = SDL_GL_CreateContext(window);

	if(!gl_context) {
		fprintf(stderr, "Failed to initialize OpenGL3 context!\n");
		return 1;
	}

	SDL_GL_MakeCurrent(window, gl_context);

	// setup GrContext
	auto glInterface = GrGLMakeNativeInterface();

	// setup contexts
	sk_sp<GrDirectContext> gr_context(GrDirectContext::MakeGL(glInterface));
	SkASSERT(gr_context);

	GrGLint buffer;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &buffer);

	GrGLFramebufferInfo buffer_info;
	buffer_info.fFBOID  = (GrGLuint)buffer;
	buffer_info.fFormat = GL_RGBA8;

	GrBackendRenderTarget target(w, h, 0, 0, buffer_info);

	SkSurfaceProps props;
	sk_sp<SkSurface> surface = SkSurface::MakeFromBackendRenderTarget(gr_context.get(), target,
		kBottomLeft_GrSurfaceOrigin, kRGBA_8888_SkColorType, nullptr, &props);
	SkCanvas* canvas         = surface->getCanvas();

	auto choice       = std::string("../F7KPMJKHG.bcd");
	auto assetsFolder = "../marioedit";
	auto log          = false;
	auto overworld    = true;

	uintmax_t filesize = std::filesystem::file_size(choice);
	std::string content;

	if(log) {
		fmt::print("Level filesize is {}\n", filesize);
	}

	if(filesize == 0x5C000) {
		if(log) {
			puts("File is encrypted");
		}

		std::ifstream ifs(choice, std::ios::binary);
		std::string input(
			(std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
		LevelParser::DecryptLevelData(input, content);
	} else {
		// First, check if compressed
		FILE* magicFile = fopen(choice.c_str(), "rb");
		uint8_t zlibMagic[2];
		uint8_t validZlibMagic1[2] = { 0x78, 0x01 };
		uint8_t validZlibMagic2[2] = { 0x78, 0x5E };
		uint8_t validZlibMagic3[2] = { 0x78, 0x9C };
		uint8_t validZlibMagic4[2] = { 0x78, 0xDA };
		fread(zlibMagic, 2, 1, magicFile);
		fclose(magicFile);
		if(memcmp(zlibMagic, validZlibMagic1, 2) == 0 || memcmp(zlibMagic, validZlibMagic2, 2) == 0
			|| memcmp(zlibMagic, validZlibMagic3, 2) == 0
			|| memcmp(zlibMagic, validZlibMagic4, 2) == 0) {
			if(log) {
				puts("File is compressed");
			}

			// Is compressed, decompress and write to new file
			std::ifstream readFile(choice, std::ios::in | std::ios::binary);
			std::vector<uint8_t> data(
				(std::istreambuf_iterator<char>(readFile)), std::istreambuf_iterator<char>());
			readFile.close();

			content = gzip::decompress((const char*)data.data(), data.size());
		} else {
			puts("File is uncompressed or an unknown format");
		}
	}

	LevelParser* levelParser = new LevelParser();

	if(content.size() == 0x5BFC0) {
#ifdef _WIN32
		SetConsoleOutputCP(CP_UTF8);
#endif

		if(log) {
			fmt::print("Assets folder: {}\n", assetsFolder);
		}
		levelParser->LoadLevelData(content, overworld);
	} else {
		puts("File is not a level");
	}

	LevelDrawer* drawer = new LevelDrawer(*levelParser, 16);
	drawer->Setup();
	drawer->SetIsOverworld(overworld);
	drawer->SetLog(log);
	drawer->SetAssetFolder(assetsFolder);

	if(log) {
		fmt::print("Width: {}\nHeight: {}\n", drawer->GetWidth(), drawer->GetHeight());
	}

	drawer->SetGraphics(canvas);
	drawer->LoadTilesheet();

	bool currentlyDragging = false;
	int dragStartX         = 0;
	int screenOffsetXStart = 0;
	int screenOffsetX      = 0;
	int dragStartY         = 0;
	int screenOffsetYStart = 0;
	int screenOffsetY      = 0;
	while(true) {
#ifndef ANDROID
		SDL_Event event;
		while(SDL_PollEvent(&event)) {
			switch(event.type) {
			case SDL_KEYDOWN: {
				SDL_Keycode key = event.key.keysym.sym;
				if(key == SDLK_ESCAPE) {
					return false;
				}
				break;
			}
			case SDL_QUIT:
				return false;
			default:
				break;
			}
		}

		float x, y;
		SDL_PumpEvents();
		uint32_t buttons = SDL_GetMouseState(&x, &y);
		if((buttons & SDL_BUTTON_LMASK) != 0) {
			if(!currentlyDragging) {
				dragStartX         = x;
				screenOffsetXStart = screenOffsetX;
				dragStartY         = y;
				screenOffsetYStart = screenOffsetY;
				currentlyDragging  = true;
			} else {
				screenOffsetX = screenOffsetXStart + x - dragStartX;
				screenOffsetY = screenOffsetYStart + y - dragStartY;
				drawer->SetOffsetX(screenOffsetX);
				drawer->SetOffsetY(screenOffsetY);
			}
		} else {
			currentlyDragging = false;
		}

#endif

		canvas->clear(SK_ColorBLACK);
		DrawMap(drawer);

		canvas->flush();

#ifndef ANDROID
		SDL_GL_SwapWindow(window);
#endif

		std::this_thread::sleep_for(std::chrono::milliseconds(16));
	}
}