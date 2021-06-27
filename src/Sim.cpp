#include "Sim.h"
#include <cmath>
#include <ctime>
#include <iostream>

/* TODO
 *  [-] Anything phsyics related needs to be moved to the phsyobj class
 */

#ifdef DEBUG
  #define debug true
#else
  #define debug false
#endif

//~~Sim Class~~//

//~Static Data Members~//
const float Sim::TIME_SCALE = 0.005f;

//~Constructors~//
Sim::Sim()
{
  sAppName = "Sim";

  Balls.reserve(10);
}

//~Methods~//
bool Sim::OnUserCreate()
{
  // Called once at the start, so create things here;
  srand(time(0));

  Balls.push_back(
          Ball(
          (rand() % PhysObj::MOD_VERT) + PhysObj::MIN_VERT,       //random polygon size between 3 and 16
          PhysObj::COLOR_SET[rand()%PhysObj::COLOR_SET.size()],            //random color
          (rand() % PhysObj::MOD_RADIUS) + PhysObj::MIN_RADIUS            //random Radius size between 5 and 30
  ));

  return true;
}

bool Sim::OnUserUpdate(float ElapsedTime)      // Called once a frame
{
  //create a background
  Clear(olc::BLACK);

  //and add a new ball everytime ENTER is pressed
  if (GetKey(olc::ENTER).bPressed)
    Balls.push_back(
            Ball(
                    (rand() % PhysObj::MOD_VERT) + PhysObj::MIN_VERT,       //random polygon size between 3 and 16
                    PhysObj::COLOR_SET[rand()%PhysObj::COLOR_SET.size()],            //random color
                    (rand() % PhysObj::MOD_RADIUS) + PhysObj::MIN_RADIUS            //random Radius size between 5 and 30
    ));

  if (GetKey(olc::BACK).bPressed && Balls.size() >0) Balls.pop_back();  //remove a ball everytime backspace is pressed

  //Update and draw the balls
  for (int i = 0; i < Balls.size(); i++)
  {
    for (int x = i+1; x < Balls.size(); x++)
    {
      if (x != i) Ball::BallCollision(Balls[i], Balls[x]);
    }
    ArenaCollision(Balls[i]);
    GetUserInput(Balls[i]);
    Balls[i].RunKinematics(ElapsedTime);
    DrawBall(Balls[i]);
  }

  DisplayComponents(Balls);
  if (debug) DebugConsole();

  //create a border around the screen
  DrawBorder(5, olc::WHITE, 1);
  return true;
}

void Sim::DrawBorder(int32_t width, olc::Pixel color, bool bottom)
{
  for(int x = 0; x < width; ++x)
  {
    DrawLine(x, width, x, ScreenHeight() - width, color);
    DrawLine(x + ScreenWidth() - width, width,
      x + ScreenWidth() - width, ScreenHeight() - width, color);
    DrawLine(0, x, ScreenWidth(), x, color);
    if (bottom)   // a leftover from when this was a breakout clone
    {
      DrawLine(0, x + ScreenHeight() - width,
          ScreenWidth(), x + ScreenHeight() - width, color);
    }
  }
}

void Sim::DrawBall(Ball& b)
{
  std::vector<olc::vd2d> Vertices = b.GetVertices();

  for (int i = 0; i < Vertices.size(); i++)             //should use VertexIndex to draw this now
  {
    if (i != b.GetVertices().size() -1)
      FillTriangle(b.GetPosition(), Vertices[i], Vertices[i+1], b.GetColor());
    else
      FillTriangle(b.GetPosition(), Vertices[i], Vertices[0], b.GetColor());
  }

  DrawCircle(b.GetPosition(), static_cast<int32_t>(b.GetRadius()));

  //Draw the velocity and acceleration vectors
  //below is a temporary fix for the velocity vectors being drawn in reverse when there are an even number of balls
  //fixed?
  if (Balls.size() % 2 != 0)
    DrawLine(b.GetPosition(), b.GetVelocity() + b.GetPosition(), olc::RED);
  else
    DrawLine(b.GetPosition(), b.GetPosition() - b.GetVelocity(), olc::RED);
  DrawLine(b.GetPosition(), b.GetAcceleration() + b.GetPosition(), olc::GREEN);

  //Draw Path
  for (olc::vf2d const &pixel : b.GetPathTrace())
  {
    Draw(pixel , b.GetColor());
  }

}

void Sim::ArenaCollision(Ball &b)
{
  //Crude arena detection
  if (b.GetPosition().y <= 5.0f + b.GetRadius()) b.ReverseVel(0, 1);
  if (b.GetPosition().x <= 5.0f + b.GetRadius()) b.ReverseVel(1, 0);
  if (b.GetPosition().x >= float(ScreenWidth()) -(5.0f) - b.GetRadius()) b.ReverseVel(1, 0);
  if (b.GetPosition().y >= float(ScreenHeight()) -(5.0f) - b.GetRadius()) b.ReverseVel(0, 1);

  //Check ball off screen
  if (b.GetPosition().y > ScreenHeight() || b.GetPosition().x > ScreenWidth())
  {
    //Reset Ball location
    b.SetPosition( 100.0f, 50.0f );
    b.ConstVertices();
    //Choose random direction
    float fAngle = (float(rand()) / float(RAND_MAX)) * 2.0f * 3.14f;
    b.SetVelocity( 50.0f * float(cos(fAngle)), 50.0f * float(sin(fAngle)));
  }
}

void Sim::DisplayComponents(std::vector<Ball>& Vb)
{
  //Toggle the velocity and position display when k is pressed
  const int XSTART=6, Y_VEL_START=15, Y_POS_START=23, YSTEP=15;

  if (GetKey(olc::K).bPressed) PhysObj::FlipDrawComponents();
  if (PhysObj::GetDrawComponents())
  {
    for (int i = 0; i < Vb.size(); i++)
    {
      DrawString(XSTART, Y_VEL_START+(YSTEP*i), "V: " + Vb[i].GetVelocity().str(), Vb[i].GetColor());
      DrawString(XSTART, Y_POS_START+(YSTEP*i), "P: " + Vb[i].GetPosition().str(), Vb[i].GetColor());
    }
  }
}

void Sim::GetUserInput(Ball& b)
{
  //Change the velocity of the ball with arrow keys
  if (GetKey(olc::UP).bHeld)
  {
    b.IncrementVel( (b.GetVelocity().x > 0) ? .5f : -.5f , 0 );
    b.IncrementVel( 0, (b.GetVelocity().y > 0) ? .5f : -.5f );
  }
  if (GetKey(olc::DOWN).bHeld)
  {
    b.IncrementVel( (b.GetVelocity().x < 0) ? .5f : -.5f , 0 );
    b.IncrementVel( 0, (b.GetVelocity().y < 0) ? .5f : -.5f );
  }

  //set acceleration to 0 if shift is held
  if (GetKey(olc::SHIFT).bHeld) b.SetAcceleration( 0.0f, 0.0f );
  else b.SetAcceleration();

  //rotate on ctrl
  if (GetKey(olc::CTRL).bHeld) b.Rotate(b.GetPosition());

  //Change the Radius and position of the ball with mouse if mouse inside ball
  if (GetMouse(0).bHeld &&
      float(GetMouseX()) >= (b.GetPosition().x - b.GetRadius()) &&
      float(GetMouseX()) <= (b.GetPosition().x + b.GetRadius()) &&
      float(GetMouseY()) >= (b.GetPosition().y - b.GetRadius()) &&
      float(GetMouseY()) <= (b.GetPosition().y + b.GetRadius()))
  {
    b.SetPosition( float(GetMouseX()), float(GetMouseY()) );
    b.ConstVertices();
    b.SetAcceleration( 0, 0 );
  }
  if (GetMouseWheel() > 0 &&
      float(GetMouseX()) >= (b.GetPosition().x - b.GetRadius()) &&
      float(GetMouseX()) <= (b.GetPosition().x + b.GetRadius()) &&
      float(GetMouseY()) >= (b.GetPosition().y - b.GetRadius()) &&
      float(GetMouseY()) <= (b.GetPosition().y + b.GetRadius()))
  {
    b.SetRadius(b.GetRadius() + 1.0f);
    b.ConstVertices();
  }
  if (GetMouseWheel() < 0 &&
      float(GetMouseX()) >= (b.GetPosition().x - b.GetRadius()) &&
      float(GetMouseX()) <= (b.GetPosition().x + b.GetRadius()) &&
      float(GetMouseY()) >= (b.GetPosition().y - b.GetRadius()) &&
      float(GetMouseY()) <= (b.GetPosition().y + b.GetRadius()))
  {
    b.SetRadius(b.GetRadius() - 1.0f);
    b.ConstVertices();
  }
}

void Sim::DebugConsole() {
  static float NetTime = 0;
  NetTime += GetElapsedTime();    //tracks the total time elapsed since start

  //do the following 100 times a second
  if (static_cast<int>(NetTime * 100) % 100 == 0) {
    system("clear");
    std::cout << "\n\tElapsed time: " << NetTime
    for (int i = 0; i < Balls.size(); i++) {
      std::cout << "Ball " << i << ":"
                << "\n\tPos: " << Balls[i].GetPosition()
                << "\n\tVel: " << Balls[i].GetVelocity()
                << "\n\tAcc: " << Balls[i].GetAcceleration()
                << "\n\tRadius: " << Balls[i].GetRadius()
                << "\n\tNumber of Vertices: " << Balls[i].GetNumberOfVertices()
                << std::endl;
    }
  }
}
