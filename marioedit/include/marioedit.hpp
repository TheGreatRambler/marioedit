#pragma once

#include <marioedit/drawer.hpp>

#include <cmath>
#include <cstdint>
#include <string>
#include <unordered_set>
#include <vector>

namespace MarioEdit {
	namespace Viewer {
		int Start();
		void DrawMap(Level::Drawer* drawer);
	}
}