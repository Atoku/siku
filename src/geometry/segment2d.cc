/*!

  \file segment.cc

  \brief Segment library implementation

*/

#include "segment.hh"

bool segment_intersect ( const Point2d& A0, const Point2d& A1,
                         const Point2d& B0, const Point2d& B1,
                         Point2d& X )
{
  double s, t;                  // parameters of the params eq.
  double num, denom;            // numerator and denominator

  denom =
    A0.getx() * ( B1.gety() - B0.gety() ) +
    A1.getx() * ( B0.gety() - B1.gety() ) +
    B1.getx() * ( A1.gety() - A0.gety() ) +
    B0.getx() * ( A0.gety() - A1.gety() );

  // num =
  //   -
  //   A0.getx() * (

  return true;
}
