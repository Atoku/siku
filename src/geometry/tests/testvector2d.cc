/*!

  \file testvector.cc

  \brief Simple test for vector2d class

*/

#include <iostream>
using namespace std;

#include "vector2d.hh"

int main()
{
  cout << "--------- Test for Vector2d  -----------" << endl;

  const size_t S = 4;
  Vector2d vec[ S ];
  Vector2d vec1 ( 1, 2 );
  Vector2d vec2;

  vec[0] = vec1;
  vec[1] = 1;

  for ( size_t i = 0; i < S; ++i  )
    {
      vec[i].print();
    }

  Vector2d_print( vec[0] );
  Vector2d_printf( vec[1], "%2.2f" );

  cout << "\n---------- copied from old test --------" << endl;

  Vector2d v1( 1, 2 );
  Vector2d v2( 3, 4 );
  Vector2d v3( 5, 6 );
  Vector2d v4( 5, 5 );

  Vector2d_print( v1 );
  Vector2d_print( v2 );
  Vector2d_print( v3 );
  Vector2d_print( v4 );

  Vector2d_print( v1 + v2 );
  Vector2d_print( v2 -= v3 );
  print( v2 );  //compare this and previous line

  V2print( v1 *= 2. );

  Vector2d_print( v3 / 3. );

  cout<< "\nv2 * v1:\n" << v2 * v1 << endl;

  // ----------

  cout<< "\ncross( v3, v1) :\n" << cross( v3, v1 ) << endl;
  cout<< "\ncross( v1, v3) :\n" << cross( v1, v3 ) << endl;

  Vector2d_print( ort( v4 ) );

  cout<< "\nabs( v4 ) :\n" << abs( v4 ) << endl;

  cout<< "\nabs2( v4 ) :\n" << abs2( v4 ) << endl;

  Vector2d_print( renorm( v4, 5 ) );

  cout<< "\nabs( v4 ) :\n" << abs( v4 ) << endl;

  cout<< "\n(bool)v1; (bool)(v1.renorm( 0 ));\n";
  cout<< (bool)v1 << "\t";
  cout<< (bool)(v1.renorm( 0 )) << endl;

  return 0;
}
