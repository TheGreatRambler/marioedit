#include <marioedit/data.hpp>
#include <marioedit/ui.hpp>

int MarioEdit::Viewer::DrawObjectSelect(
	SkCanvas* canvas, Level::Drawer* drawer, int mouse_x, int mouse_y) {
	drawer->SetOffsetX(0);
	drawer->SetOffsetY(0);

	constexpr int NUM_WIDTH = 10;

	int current_x = 0;
	int current_y = 0;
	int selected  = 0;
	for(int i = 0; i < Level::Data::ALL_OBJS.size(); i++) {
		if(i != 0 && i % NUM_WIDTH == 0) {
			current_y += 32;
			current_x = 0;
		}

		drawer->DrawImage(
			Level::Data::ALL_OBJS[i] | Level::Data::SMB1, current_x, current_y, 32, 32);

		if(mouse_x >= current_x && mouse_x < current_x + 32 && mouse_y >= current_y
			&& mouse_y < current_y + 32) {
			selected = Level::Data::ALL_OBJS[i];
		}

		current_x += 32;
	}

	return selected;
}