#pragma once

#include <marioedit/drawer.hpp>

#include <core/SkCanvas.h>

namespace MarioEdit {
	namespace Viewer {
		int Start();
		int DrawObjectSelect(SkCanvas* canvas, Level::Drawer* drawer, int mouse_x, int mouse_y);
	}
}