/* Where the rendering happens */

#pragma once
#include "olcPixelGameEngine.h"
#include "PhysObj.h"

/* TODO:
 *  [] move the constants in displaycomponents to statics of the class
 *  make them dynamic and derrived from the screen hight/widht method
 */

class Ball;

class Sim : public olc::PixelGameEngine
{
public:
	Sim();

  bool OnUserCreate() override;
  bool OnUserUpdate(float fElapsedTime) override;
  void DrawBorder(int32_t width, olc::Pixel color, bool bottom);
  void DrawBall(Ball &b);
  void ArenaCollision(Ball &b);
  void GetUserInput(Ball &b);
  void DisplayComponents(std::vector<Ball> &Vb);
  void DebugConsole();

  static const float TIME_SCALE;              //used to make events run in real time rather than frames

private:
  std::vector<Ball> Balls;
  bool PrintDebugConsole = false;
};

