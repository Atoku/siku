/*!

  \file segment2d.cc

  \brief Segment library implementation

*/

#include "segment2d.hh"

namespace Geometry
{
// ~~~~~~~~~~~~~~~~~~~~~~~~~~ local utils ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

  // returns point on line given by two points, which is calculated as a
  // scaled vector between those two points
  inline pnt2d pnt_on_line ( const pnt2d& p1, const pnt2d& p2, const double& d )
  {
    return p1 + d * ( p2 - p1 );
  }

// ~~~~~~~~~~~~~~~~~~~~ segment2d implementations ~~~~~~~~~~~~~~~~~~~~~~~~~~~

  int line_inter( const pnt2d& a1, const pnt2d& a2,
                        const pnt2d& b1, const pnt2d& b2, pnt2d& X )
  {
      // algorithm taken from http://algolist.manual.ru/maths/geom/intersect/lineline2d.php

    vec2d a = a2 - a1, b = b2 - b1, ab = a1 - b1;

    double d = cross( b, a ); // denominator, signifies if lines are parallel
    double na = cross ( b, ab ); // numerator of a
    double nb = cross( a, ab ); // numerator of b


    if ( d == 0. )  // lines are parallel
      {
        if ( na == 0. || nb == 0. )  // lines are the same
          {
            X = ( a1 + a2 + b1 + b2 ) / 4.; // middle point
            return 2;
          }
        else
          return 0;  // lines don`t cross
      }
    else  // lines are NOT parallel
      {
        double ua = na / d;
        //double ub = nb / d;
        X = pnt_on_line ( a1, a2, ua );
        return 1;
      }
    return 0;
  }

  bool line_seg_inter( const pnt2d& a1, const pnt2d& a2,
                        const pnt2d& b1, const pnt2d& b2, pnt2d& X )
  {
      // algorithm taken from http://algolist.manual.ru/maths/geom/intersect/lineline2d.php

      vec2d a = a2 - a1, b = b2 - b1, ab = a1 - b1;

      // denominator, signifies if lines are parallel
      double d = cross( b, a );
  //        ( b2.y - b1.y ) * ( a2.x - a1.x )
  //        - ( b2.x - b1.x ) * ( a2.y - a1.y );

      // numerator of a
      double na = cross ( b, -ab );
  //        ( b2.x - b1.x ) * ( a1.y - b1.y )
  //        - ( b2.y - b1.y ) * ( a1.x - b1.x );
      // numerator of b
      double nb = cross( a, -ab );
  //        ( a2.x - a1.x ) * ( a1.y - b1.y )
  //        - ( a2.y - a1.y ) * ( a1.x - b1.x );

      if ( d == 0. )  // lines are parallel
        {
          if ( na == 0. || nb == 0. )  // lines are the same
            {
              double aa;  // a1-a2 distance
              double ab1;  // a1-b1 distance
              double ab2;  // a1-b2 distance

              if ( a1.getx() == a2.getx() )  // if lines are vertical - set interposition
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
                    { X = pnt_on_line ( a1, a2, ab2 / ( 2. * aa ) );
                    return true; }
                  if ( ab2 >= aa )  // 'a' within 'b'
                    { X = pnt_on_line ( a1, a2, 0.5 );
                    return true; }
                }
              if ( ab1 > 0. && ab1 < aa )  // b1 within 'a' segment
                {
                  if ( ab2 <= 0. )  // b2 backwards from a1
                    { X = pnt_on_line ( a1, a2, ab1 / ( 2. * aa ) );
                    return true; }
                  if ( ab2 > 0. && ab2 < aa )  // b2 within 'a' segment
                    { X = pnt_on_line ( a1, a2, ( ab1 + ab2 ) / ( 2. * aa ) );
                    return true; }
                  if ( ab2 >= aa )  // b2 further than a2
                    { X = pnt_on_line ( a1, a2, ( aa + ab1 ) / ( 2. * aa ) );
                    return true; }
                }
              if ( ab1 >= aa )  // b1 further than a2
                {
                  if ( ab2 <= 0. )  // 'a' within 'b'
                    { X = pnt_on_line ( a1, a2, 0.5 );
                    return true; }
                  if ( ab2 > 0. && ab2 < aa )  // b2 within 'a' segment
                    { X = pnt_on_line ( a1, a2, ( aa + ab2 ) / ( 2. * aa ) );
                    return true; }
                  if ( ab2 >= aa )  // 'b' further then a2
                    return false;
                }
            }
          else
            return false;  // lines don`t cross
        }
      else  // lines are NOT parallel
        {
          double ua = na / d;
          double ub = nb / d;

          // intersection located within segments
          if ( ua >= 0. && ua <= 1. && ub >= 0. && ub <= 1. )
            {
              X = pnt_on_line ( a1, a2, ua );
              return true;
            }
        }
      return false;
    }

// --------------------------------------------------------------------------
  
  /*! \brief Determine the intersection of two segments defined by their
      vertices

      \param[in] A0 1st vertex of the 1st segment
      \param[in] A1 2st vertex of the 1st segment
      \param[in] B0 1st vertex of the 2st segment
      \param[in] B1 2st vertex of the 2st segment
      \param[out] X intersection point. Underfined if return value is
                 false

      \return true if segments intersect, false otherwise

   */
  bool segment2d_intersect ( const pnt2d& A0, const pnt2d& A1,
                             const pnt2d& B0, const pnt2d& B1,
                             pnt2d& X )
  {
    vec2d a(A1-A0), b(B1-B0), c(B0-A0); // connection vectors

    double D1, D2, D;             // Cramer's rule determinants
    double s, t;                  // parameters of the params eq.

    D = b.cross(a);

    // break if denominator 0, parallel guys.
    if ( D == 0 ) return false;

    D1 = b.cross(c);
    D2 = a.cross(c);

    s = D1 / D;
    t = D2 / D;

    X = A0 + s * a;
    if ( s < 0 || 1 < s || t < 0 || 1 < t ) return false;

    return true;
  }

}
