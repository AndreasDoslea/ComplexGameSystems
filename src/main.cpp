#include "BasicNetworkingApplication.h"
#include <GLFW/glfw3.h>
#include <time.h>

int main() {
	srand(time(0));
	BaseApplication* app = new BasicNetworkingApplication();
	if (app->startup())
		app->run();
	app->shutdown();

	return 0;
}