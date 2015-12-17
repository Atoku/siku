/*!

  \file testsegment2d.cc

  \brief Testing segment2d library

*/

/*
 * Siku: Discrete element method sea-ice model: siku.cc
 *
 * Copyright (C) 2015-2016 UAF.
 * Authors: Anton Kulchitsky, Gleb Velikhovskiy
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */

#include <iostream>
using namespace std;

#include "point2d.hh"
#include "segment2d.hh"

int main()
{
  cout << "Testing segment2d library" << endl;

  Point2d A, B, C, D;
  Point2d X;                    // result

  // 1. Simple case
  
  A = { 1, 0 };
  B = {-1, 0 };

  C = { 0.5, -1};
  D = { 0.5, 1 };
  
  bool is_intersect = segment2d_intersect ( A, B, C, D, X );

  cout << "Intersect? " << is_intersect << endl;
  Point2d_print( X );
  cout << "Correct answer: yes, " << " X = ( 0.5, 0 ) " << endl;

  // 2. Rather random example of intersecting segments
  
  A = { 2, 2 };
  B = { 0, -1};

  C = { 0, 2 };
  D = { 2, 1 };

  is_intersect = segment2d_intersect( A, B, C, D, X );

  cout << "Intersect? " << is_intersect << endl;
  Point2d_print( X );
  cout << "Correct answer: yes, " << " X = ( 1.5, 1.25 ) " << endl;
  
  return 0;
}
