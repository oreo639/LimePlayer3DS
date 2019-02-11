#include <stdio.h>

#include "app.hpp"

int main(int argc, char **argv) {
	App *app = new App();
	while (app->MainLoop() == 1) {
		app->Update();
		app->Draw();
	}

	delete app;
	return 0;
}
