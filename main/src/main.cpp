#include <CLI/App.hpp>
#include <CLI/Config.hpp>
#include <CLI/Formatter.hpp>
#include <chrono>
#include <fmt/core.h>
#include <fstream>
#include <marioedit.hpp>
#include <ostream>
#include <thread>

int main(int argc, char** argv) {
	MarioEdit::Viewer::Start();
	return 0;
}