/*!

  \file segment2d.cc

  \brief Segment library implementation

*/

#include "segment2d.hh"

namespace Geometry
{

// ~~~~~~~~~~~~~~~~~~~~ segment2d implementations ~~~~~~~~~~~~~~~~~~~~~~~~~~~


  bool segment2d_intersect( const pnt2d& a1, const pnt2d& a2,
                        const pnt2d& b1, const pnt2d& b2, pnt2d& X )
  {
      // algorithm taken from
      // http://algolist.manual.ru/maths/geom/intersect/lineline2d.php

      vec2d a = a2 - a1, b = b2 - b1, ab = b1 - a1;

      // Cramer's rule determinants
      double D = cross( b, a ),  // signifies if lines are parallel
            Da = cross( b, ab ),
            Db = cross( a, ab );

      if ( D == 0. )  // lines are parallel
        {
          if ( Da == 0. || Db == 0. )  // lines are the same
            {
              double aa;  // a1-a2 distance
              double ab1;  // a1-b1 distance
              double ab2;  // a1-b2 distance

              if ( a1.getx() == a2.getx() )  // if lines are vertical -
                                             // set interposition
                {
                  aa = a2.gety() - a1.gety();
                  ab1 = b1.gety() - a1.gety();
                  ab2 = b2.gety() - a1.gety();
                }
              else  // same for non-vertical lines
                {
                  aa = a2.getx() - a1.getx();
                  ab1 = b1.getx() - a1.getx();
                  ab2 = b2.getx() - a1.getx();
                }

              // check interposition and return according to it
              if ( ab1 <= 0. )  // b1 backwards from a1
                {
                  if ( ab2 <= 0. )  // 'b' backwards from a1
                    return false;
                  if ( ab2 > 0. && ab2 < aa )  // b2 within 'a'
                    {
                      X = a1 + a * ( ab2 / ( 2. * aa ) );
                      return true;
                    }
                  if ( ab2 >= aa )  // 'a' within 'b'
                    {
                      X = a1 + a * 0.5;
                      return true;
                    }
                }
              if ( ab1 > 0. && ab1 < aa )  // b1 within 'a' segment
                {
                  if ( ab2 <= 0. )  // b2 backwards from a1
                    {
                      X = a1 + a * ( ab1 / ( 2. * aa ) );
                      return true;
                    }
                  if ( ab2 > 0. && ab2 < aa )  // b2 within 'a' segment
                    {
                      X = a1 + a * ( ( ab1 + ab2 ) / ( 2. * aa ) );
                      return true;
                    }
                  if ( ab2 >= aa )  // b2 further than a2
                    {
                      X = a1 + a * ( ( aa + ab1 ) / ( 2. * aa ) );
                      return true;
                    }
                }
              if ( ab1 >= aa )  // b1 further than a2
                {
                  if ( ab2 <= 0. )  // 'a' within 'b'
                    {
                      X = a1 + a * 0.5;
                      return true;
                    }
                  if ( ab2 > 0. && ab2 < aa )  // b2 within 'a' segment
                    {
                      X = a1 + a * ( ( aa + ab2 ) / ( 2. * aa ) );
                      return true;
                    }
                  if ( ab2 >= aa )  // 'b' further then a2
                    return false;
                }
            }
          else
            return false;  // lines don`t cross
        }
      else  // lines are NOT parallel
        {
          double pa = Da / D;
          double pb = Db / D;

          // intersection located within segments
          if ( pa >= 0. && pa <= 1. && pb >= 0. && pb <= 1. )
            {
              X = a1 + pa * a;
              return true;
            }
        }
      return false;
    }

// --------------------------------------------------------------------------
  
  double segment2d_distance( const pnt2d& a1, const pnt2d& a2,
                             const pnt2d& b1, const pnt2d& b2 )
  {
    pnt2d X;
    if( segment2d_intersect( a1, a2, b1, b2, X ) )
      return 0.;

    // optimized minimal distance between vertices
    return sqrt( min( min( abs2(a1-b1), abs2(a1-b2) ),
                      min( abs2(a2-b1), abs2(a2-b2) ) ) );
  }

}
