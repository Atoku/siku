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

//#include "point2d.hh"
#include "segment2d.hh"

void INTER_RES( int n, bool i, vec2d x )
{
  cout << "\n#" << n << ": Does intersect? - "
        << ( (i) ? "yes, result point: " : "no\n" );
  if( i )  x.print();
}


int main()
{

  cout << "Testing segment2d library" << endl;

  cout << "____________ Next section has NO intersections _____________";

  pnt2d A, B, C, D;
  pnt2d X;                    // result

  // 1. " L " figure --------------------------------------------------

  A = { 1, 0 };
  B = { 1, 3 };

  C = { 1.0001, 0.0001 };
  D = { 3, 0 };

  INTER_RES ( 1, segment2d_intersect ( A, B, C, D, X ), X );

  // 2. " L " figure --------------------------------------------------

  A = { 1, 0 };
  B = { 1, 3 };

  C = { 1, -0.0001 };
  D = { 3, 0 };

  INTER_RES ( 2, segment2d_intersect ( A, B, C, D, X ), X );

  // 3. " L " figure --------------------------------------------------

  A = { 1, 0 };
  B = { 1, 3 };

  C = { 0.9999, -0.0001 };
  D = { 3, 0 };

  INTER_RES ( 3, segment2d_intersect ( A, B, C, D, X ), X );

  // 4. " V " figure --------------------------------------------------

  A = { 1, 0 };
  B = { 1, 3 };

  C = { 1, -0.0001 };
  D = { 1.1, 3 };

  INTER_RES ( 4, segment2d_intersect ( A, B, C, D, X ), X );

  // 5. almost on one line --------------------------------------------

  A = { 1, 0 };
  B = { 1, 3 };

  C = { 0.9999, 0.0001 };
  D = { 2, -3 };

  INTER_RES ( 5, segment2d_intersect ( A, B, C, D, X ), X );

  // 6. parallel ------------------------------------------------------

  A = { 1, 0 };
  B = { 1, 3 };

  C = { 1.0001, 0 };
  D = { 1.0001, 3 };

  INTER_RES ( 6, segment2d_intersect ( A, B, C, D, X ), X );

  // 7. on one line --------------------------------------------------

  A = { 1, 0 };
  B = { 1, 3 };

  C = { 1, -0.0001};
  D = { 1, -3 };

  INTER_RES ( 7, segment2d_intersect ( A, B, C, D, X ), X );

  cout<<"_______ in next section ALL cases have intersection ____________\n";

  // 8. " L " figure --------------------------------------------------

  A = { 1, 0 };
  B = { 1, 3 };

  C = { 1, 0 };
  D = { 3, 0 };

  INTER_RES ( 8, segment2d_intersect ( A, B, C, D, X ), X );

  // 9. " + " figure --------------------------------------------------

  A = { 1, 0 };
  B = { 1, 3 };

  C = { 0, 1 };
  D = { 2, 1 };

  INTER_RES ( 9, segment2d_intersect ( A, B, C, D, X ), X );

  // 10. almost on one line -------------------------------------------

  A = { 1, 0 };
  B = { 1, 3 };

  C = { 0.9999, 0.0005};
  D = { 2, -2 };

  INTER_RES ( 10, segment2d_intersect ( A, B, C, D, X ), X );

  // 11. " V " figure --------------------------------------------------

  A = { 1, 0 };
  B = { 1, 3 };

  C = { 0.9999, 0.0001};
  D = { 2, 3 };

  INTER_RES ( 11, segment2d_intersect ( A, B, C, D, X ), X );

  // 12. " X " almost on one line --------------------------------------

  A = { 1, 0 };
  B = { 1, 3 };

  C = { 0.9999, 0 };
  D = { 1.0001, 3 };

  INTER_RES ( 12, segment2d_intersect ( A, B, C, D, X ), X );

  cout<<"\n_________ this case can either intersect or not _____________\n";

  // 13. On one line --------------------------------------------------

  A = { 1, 0 };
  B = { 1, 3 };

  C = { 1, 0 };
  D = { 1, -2 };

  cout<<"\nTwo segments on one line with mutual point:\n";
  INTER_RES ( 13, segment2d_intersect ( A, B, C, D, X ), X );

  return 0;
}
