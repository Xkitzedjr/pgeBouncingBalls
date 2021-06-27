//
// Created by chris on 3/18/21.
//
/* 2x2 Matrix Transform functions for use on olc 2d vectors */

#ifndef COLLISION_TRANSFORM_H
#define COLLISION_TRANSFORM_H

#include "olcPixelGameEngine.h"

namespace chris {

  template <class Type> struct Matrix {
    olc::v2d_generic<Type> row1;
    olc::v2d_generic<Type> row2;

    olc::v2d_generic<Type> transform(const olc::v2d_generic<Type> vect) {
      Type x = vect.dot(row1);
      Type y = vect.dot(row2);
      olc::v2d_generic<Type> result = { x, y };
      return result;
    }
  };

  typedef Matrix<int32_t> mi22;
  typedef Matrix<uint32_t> mu22;
  typedef Matrix<float> mf22;
  typedef Matrix<double> md22;

}

// to be replaced with above
olc::vi2d transform(const olc::vi2d &vect, const olc::vi2d &row1, const olc::vi2d &row2);
olc::vd2d transform(const olc::vd2d &vect, const olc::vd2d &row1, const olc::vd2d &row2);

#endif //COLLISION_TRANSFORM_H
