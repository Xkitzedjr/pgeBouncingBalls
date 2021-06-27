//
// Created by chris on 3/18/21.
//

#include "PhysObj.h"
#include "Transform.h"

#ifdef DEBUG
  #define debug true
#else
  #define debug false
#endif
//~~PhysObj Class~~//

//~Constructors~//

PhysObj::PhysObj(int NumberOfVertices, olc::Pixel Color, olc::vd2d initPos, olc::vd2d initVel, olc::vd2d initAcc) {
  this->NumberOfVertices = NumberOfVertices;
  this->Color = Color;
  this->Position = initPos;

  //Choose random direction for velocity
  float fAngle = (float(rand()) / float(RAND_MAX)) * 2.0f * 3.14f;
  this->Velocity = { initVel.x * float(cos(fAngle)), initVel.y * float(sin(fAngle)) };

  this->Acceleration = initAcc;
}

//~Static Data Members~//

const olc::vd2d PhysObj::ACC_CONST = { 0.0f, 30.0f };
const olc::vd2d PhysObj::INIT_POS = { 400.0f, 300.0f };
const olc::vd2d PhysObj::INIT_VEL = { 50.0f, 50.0f };

const int PhysObj::MAX_RADIUS = 30, PhysObj::MIN_RADIUS = 5, PhysObj::MAX_VERT = 16, PhysObj::MIN_VERT = 3;

const int PhysObj::MOD_RADIUS = PhysObj::MAX_RADIUS - PhysObj::MIN_RADIUS +1;   //for use with rand() %
const int PhysObj::MOD_VERT = PhysObj::MAX_VERT - PhysObj::MIN_VERT +1;

const std::vector<olc::Pixel> PhysObj::COLOR_SET {
        olc::GREY,
        olc::YELLOW,
        olc::RED,
        olc::GREEN,
        olc::BLUE,
        olc::MAGENTA
};

double PhysObj::LinearDragFactor = 20.0;
double PhysObj::QuadraticDragFactor = 2.0;

bool PhysObj::DrawComponents = 0;

//~Methods~//

void PhysObj::UpdatePos(float time) {
  //this is a pseudo translation transform, if a Position component is 0 there will be division by 0
  //a real translation matrix will be non square
  Position = transform(Position, {(Velocity.x * Sim::TIME_SCALE / Position.x)+1, 0}, {0, (Velocity.y * Sim::TIME_SCALE / Position.y)+1});
  for (int i = 0; i < Vertices.size(); i++) {
    Vertices[i] = transform(Vertices[i], {(Velocity.x * Sim::TIME_SCALE / Vertices[i].x)+1, 0}, {0, (Velocity.y * Sim::TIME_SCALE / Vertices[i].y)+1});
  }

  //Update PathTrace
  PathTrace.push_back(GetPosition());
  if (PathTrace.size() > 500)
    PathTrace.pop_front();
}

void PhysObj::UpdateVel(float time) {
  Velocity += Acceleration * time;
}

void PhysObj::UpdateAcc(float time, Drag drag) {
  switch (drag) {
    case (Drag::CONST) : {
      Acceleration = ACC_CONST;
      break;
    }
    case (Drag::LINEAR) : {
      Acceleration = ACC_CONST - Velocity * LinearDragFactor / Mass; // g - linear drag
      break;
    }
    case (Drag::QUADRATIC) : {
      Acceleration.x = ACC_CONST.x - sqrt(Velocity.x * Velocity.x + Velocity.y * Velocity.y) * Velocity.x * QuadraticDragFactor / Mass;
      Acceleration.y = ACC_CONST.y - sqrt(Velocity.x * Velocity.x + Velocity.y * Velocity.y) * Velocity.y * QuadraticDragFactor / Mass;
    }
  }
}

void PhysObj::RunKinematics(float time) {
  UpdateAcc(time);
  UpdateVel(time);
  UpdatePos(time);
}

void PhysObj::IncrementVel(float xIncrementFactor, float yIncrementFactor) {
  Velocity.x += xIncrementFactor * 1.0f;
  Velocity.y += yIncrementFactor * 1.0f;
}

void PhysObj::ReverseVel(bool x, bool y) {
  if(x)
    Velocity.x *= -1.0;
  if(y)
    Velocity.y *= -1.0;
}

void PhysObj::Rotate(olc::vd2d pivot, double rotationAngle) {
  using namespace chris;

  md22 rotationMatrix;
  rotationMatrix.row1 = { cos(rotationAngle), sin(rotationAngle) *(-1) };
  rotationMatrix.row2 = { sin(rotationAngle), cos(rotationAngle) };

  for ( olc::vd2d &vertex : Vertices ) {
    vertex = rotationMatrix.transform((vertex - pivot)) + pivot;
  }
}

//~~Ball Class~~//

const double Ball::MASS_FACTOR = 10.0;

//~Constructors~//

Ball::Ball(int NumberOfVertices, olc::Pixel Color, double Radius):
  PhysObj(NumberOfVertices, Color) {
  this->Radius = Radius;
  this->Mass = Ball::MASS_FACTOR * Radius;
  Ball::ConstVertices();
}

//~Methods~//

void Ball::ConstVertices()
{
  if (!Vertices.empty()) Vertices.clear();

  float angleStep = 6.28318 / float(NumberOfVertices); // 2pi over n for a regular n-gon:
  for (int i = 0; i < NumberOfVertices; i++)
  {
    //create n points around the center to make the n-gon
    float x = cos(i * angleStep);
    float y = sin(i * angleStep);
    Vertices.push_back({Radius * x, Radius * y});
  }

  //move the polygon to its initial pos
  for (int i = 0; i < NumberOfVertices; i++)
    Vertices[i] = Vertices[i] + Position;

  //check if the index vector is clear before populating
  if (!VertexIndex.empty())
    VertexIndex.clear();

  //populate the index vector, the first of each 3-tuple will be n
  //and the next two will be the vertex on the perimeter of the polygon
  for (int i = 0; i < NumberOfVertices; i++)
  {
    if (i != NumberOfVertices -1)
    {
      VertexIndex.push_back(NumberOfVertices);
      VertexIndex.push_back(i);
      VertexIndex.push_back(i+1);
    }
    else
    {
      VertexIndex.push_back(NumberOfVertices);
      VertexIndex.push_back(i);
      VertexIndex.push_back(0);
    }
  }
}

void Ball::BallCollision(Ball &b1, Ball &b2) {
  double x1 = b1.Position.x, y1 = b1.Position.y, r1 = b1.Radius;
  double x2 = b2.Position.x, y2 = b2.Position.y, r2 = b2.Radius;

  auto collided = [] (double x1, double x2, double y1, double y2, double r1, double r2) -> bool {
    // compare the distance between two balls and the sum of their radii
    return (x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2) <= (r1 + r2) * (r1 + r2);
  };

  // move the balls outside each other and reverse velocity
  if (collided(x1, x2, y1, y2, r1, r2))
  {
    double distance = sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
    double overlap = 1.5 * (distance - r1 - r2);

    b1.SetPosition(
            x1 - (overlap * (x1 - x2) / distance),
            y1 - (overlap * (y1 - y2) / distance)
    );

    b2.SetPosition(
            x2 + (overlap * (x1 - x2) / distance),
            y2 + (overlap * (y1 - y2) / distance)
    );

    // the impulse can be broken into two responses,
    // one in the direction tangent to both circles
    // and one normal to that tangent
    olc::vd2d normal = (b1.Position - b2.Position) / distance;  // divide by distance to get a unit vector, we just need the direction
    olc::vd2d tangent = normal.perp();

    // the tangent comp of the impulse
    olc::vd2d TangentImpulseOn1 = b1.Velocity.dot(tangent) * tangent;
    olc::vd2d TangentImpulseOn2 = b2.Velocity.dot(tangent) * tangent;

    // the normal comp of the impulse
    double norm1 = b1.Velocity.dot(normal);
    double norm2 = b2.Velocity.dot(normal);

    // conservation of momentum in the direction of impact, the normal direction
    auto calcMomentum = [] (double m1, double m2, double v1, double v2) -> double {
      // takes two masses and their velocities and returns the final velocity of mass 1
      return (v1 * (m1 - m2) + 2.0 * m2 * v2) / (m1 + m2);
    };

    olc::vd2d NormalImpulseOn1 = calcMomentum(b1.Mass, b2.Mass, norm1, norm2) * normal;
    olc::vd2d NormalImpulseOn2 = calcMomentum(b2.Mass, b1.Mass, norm2, norm1) * normal;

    // debug
    if (debug) {
      if (NormalImpulseOn2.mag() > 100 || NormalImpulseOn1.mag() > 100) {
        std::cout << "woah"
                  << "\n\tNormalImpulseOn1 " << NormalImpulseOn1
                  << "\n\tNormalImpulseOn2 " << NormalImpulseOn2
                  << "\n\tStill Colliding: " << collided(x1, x2, y1, y2, r1, r2) << std::endl;
      }
    }

    // add the two components
    b1.Velocity = TangentImpulseOn1 + NormalImpulseOn1;
    b2.Velocity = TangentImpulseOn2 + NormalImpulseOn2;

  }

}

void Ball::SetPosition(float x, float y) {
  if (x >= 0 && y >=0) {
    Position = olc::vd2d{ x, y };
    Ball::ConstVertices();
  }
}