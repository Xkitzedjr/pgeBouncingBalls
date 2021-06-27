//
// Created by chris on 3/18/21.
//

#include "Transform.h"

//~~Functions~~//

olc::vi2d transform(const olc::vi2d &vect, const olc::vi2d &row1, const olc::vi2d &row2)
{
  int x = vect.dot(row1);
  int y = vect.dot(row2);
  olc::vi2d result = { x, y };
  return result;
}

olc::vd2d transform(const olc::vd2d &vect, const olc::vd2d &row1, const olc::vd2d &row2)
{
  float x = vect.dot(row1);
  float y = vect.dot(row2);
  olc::vd2d result = { x, y };
  return result;
}