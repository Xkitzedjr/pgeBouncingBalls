//
// Created by chris on 3/18/21.
//

/* Where the physics happens
 *
 * TODO
 *  */


#ifndef COLLISION_PHYSOBJ_H
#define COLLISION_PHYSOBJ_H

#include "olcPixelGameEngine.h"
#include "Sim.h"

enum class Drag {CONST, LINEAR, QUADRATIC};

class PhysObj
{
public:
  // Constants
  static const olc::vd2d ACC_CONST, INIT_POS, INIT_VEL;
  static const int MAX_RADIUS, MIN_RADIUS, MOD_RADIUS, MAX_VERT, MIN_VERT, MOD_VERT;
  static const std::vector<olc::Pixel> COLOR_SET;
  static double LinearDragFactor, QuadraticDragFactor;

  // Constructor
  PhysObj(int NumberOfVertices, olc::Pixel Color, olc::vd2d initPos = INIT_POS, olc::vd2d initVel = INIT_VEL, olc::vd2d initAcc = ACC_CONST);

  // General Utility Functions
  void RunKinematics(float time);
  void UpdatePos(float time);
  void UpdateVel(float time);
  void UpdateAcc(float time, Drag drag = Drag::QUADRATIC);
  void IncrementVel(float xIncrementFactor, float yIncrementFactor);
  void ReverseVel(bool x, bool y);
  void Rotate(olc::vd2d pivot, double rotationAngle = .1);


  // Pure Virtual Utility Functions
  virtual void ConstVertices(void) = 0;

  // Accessors
  const inline olc::vd2d& GetPosition(void) const { return Position; }
  const inline olc::vd2d& GetVelocity(void) const { return Velocity; }
  const inline olc::vd2d& GetAcceleration(void) const { return Acceleration; }
  const inline olc::Pixel& GetColor(void) const { return Color; }
  const inline std::list<olc::vd2d>& GetPathTrace(void) const { return PathTrace; }
  const inline std::vector<olc::vd2d>& GetVertices(void) const { return Vertices; }
  static bool inline GetDrawComponents(void) { return DrawComponents; }
  const inline int GetNumberOfVertices(void) { return NumberOfVertices; }

  // Mutators
  void virtual SetPosition(float x, float y) { if (x >= 0 && y >=0) Position = olc::vd2d{ x, y }; }
  void inline SetVelocity(float x, float y) { Velocity = olc::vd2d{ x, y }; }
  void inline SetAcceleration(void) { Acceleration = ACC_CONST; }
  void inline SetAcceleration(float x, float y) { Acceleration = olc::vd2d{ x, y }; }
  static void inline FlipDrawComponents(void) { DrawComponents = !DrawComponents; }

protected:
  double Mass = 0;

private:
  int NumberOfVertices;
  olc::Pixel Color;
  olc::vd2d Position;
  olc::vd2d Velocity;
  olc::vd2d Acceleration;

  static bool DrawComponents;

  std::list<olc::vd2d> PathTrace;
  std::vector<olc::vd2d> Vertices;      //vertices to draw triangles which will approximate our shapes

  //Contains each triangle to draw as an ordered list of 2-tuples, each int representing the index
  //of the vertex in Vertices. ie if the first 2 numbers are 0,1,2 then the first triangle to
  //be drawn will be made up of the -1th 1st and 2nd vertices in Vertices
  //Vertices largest index will be NoVertices -2, so an index value of NoVertices represents
  //the center of the polygon
  std::vector<int> VertexIndex; //is populated but not used

friend class Ball;
};

class Ball : public PhysObj
{
public:
  static const double MASS_FACTOR;

  // Constructor
  Ball(int NumberOfVertices, olc::Pixel Color, double Radius);

  // Unique Utility Functions
  static void BallCollision(Ball &ball1, Ball &ball2);
  void SetPosition(float x, float y) override;

  // Derived Utility Functions
  void ConstVertices(void) override;    // used every time the position is changed to reorient the vertices around the position

  // Accessors
  double GetRadius() { return Radius; }

  // Mutators
  void SetRadius(double newRadius) { if(newRadius > 1.0) Radius = newRadius; Mass = MASS_FACTOR * Radius; }

private:
  double Radius;
};

#endif //COLLISION_PHYSOBJ_H
