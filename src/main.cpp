#include "Sim.h"
#include <string>
#include <vector>

#define S_WIDTH 800
#define S_HEIGHT 600
#define P_WIDTH 1
#define P_HEIGHT 1

int main(int argc, char* argv[])
{
  int32_t screenWidth, screenHeight, pixelWidth, pixelHeight;

  if (argc == 5) {
    std::vector<std::string> args(argv+1, argv + argc);
    screenWidth = std::stoi(args[0]);
    screenHeight = std::stoi(args[1]);
    pixelWidth = std::stoi(args[2]);
    pixelHeight = std::stoi(args[3]);
  }

  else if (argc != 1) {
    std::cout << "Usage: <screen_w>, <screen_h>, <pixel_w>, <pixel_h>\n"
      << "DEFAULTS: S_WIDTH " << S_WIDTH << " S_HEIGHT " << S_HEIGHT
      << " P_WIDTH " << P_WIDTH << " P_HEIGHT " << P_HEIGHT << std::endl;

  }

  else {
    screenWidth = (int32_t)S_WIDTH;
    screenHeight = (int32_t)S_HEIGHT;
    pixelWidth = (int32_t)P_WIDTH;
    pixelHeight = (int32_t)P_HEIGHT;
  }

	Sim demo;
	if (demo.Construct(screenWidth, screenHeight, pixelWidth, pixelHeight)) //x1 &x2 are the actual resolution the prog will run at, x3, x4 are how many real pixes an ingame pixel will be
		demo.Start();

	return 0;
}

