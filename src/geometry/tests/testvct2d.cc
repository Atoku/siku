/*!

  \file testvector.cc

  \brief Simple test for vec2d class

*/

#include <iostream>
using namespace std;

#include "vec2d.hh"

int main()
{
  cout << "--------- Test for vec2d  -----------" << endl;

  const size_t S = 4;
  vec2d vec[ S ];
  vec2d vec1 ( 1, 2 );
  vec2d vec2;

  vec[0] = vec1;
  vec[1] = 1;

  for ( size_t i = 0; i < S; ++i  )
    {
      vec[i].print();
    }

  vec2d_print( vec[0] );
  vec2d_printf( vec[1], "%2.2f" );

  cout << "\n---------- copied from old test --------" << endl;

  vec2d v1( 1, 2 );
  vec2d v2( 3, 4 );
  vec2d v3( 5, 6 );
  vec2d v4( 5, 5 );

  vec2d_print( v1 );
  vec2d_print( v2 );
  vec2d_print( v3 );
  vec2d_print( v4 );

  cout<<"\nv1 + v2, correct: ( 4, 6 ):\n";
  print( v1 + v2 );
  cout<<"\nv2 -= v3, correct: ( -2, -2 ): \n";
  print( v2 -= v3 );

  cout<<"\nv1 *= 2, correct: ( 2, 4 ):\n";
  print( v1 *= 2. );

  cout<<"\n v3 / 3., correct: ( 1.6.., 2 ):\n";
  print( v3 / 3. );

  cout<< "\nv2 * v1, correct: 4 :\n"
      << v2 * v1 << endl;

  // ----------

  cout<< "\ncross( v3, v1 ), correct 8 :\n"
      << cross( v3, v1 ) << endl;
  cout<< "\ncross( v1, v3 ), correct -8 :\n"
      << cross( v1, v3 ) << endl;

  cout<<"\nort( v3), correct: ( 1.41.., 1.41.. ) :\n";
  print( ort( v4 ) );

  cout<< "\nabs( v4 ), correct: 7.0.. :\n"
      << abs( v4 ) << endl;

  cout<< "\nabs2( v4 ), correct: 50 :\n"
      << abs2( v4 ) << endl;

  cout<<"\nrenorm( v3, 5 ), correct: ( 3.5.., 3.5.. ) :\n";
  print( renorm( v4, 5 ) );

  cout<< "\nabs( v4 ), correct: 5 :\n"
      << abs( v4 ) << endl;

  cout<< "\n(bool)v1; (bool)(v1.renorm( 0 )), correct: 1, 0 :\n";
  cout<< (bool)v1 << "\t";
  cout<< (bool)(v1.renorm( 0 )) << endl;

  return 0;
}
