/*!

  \file segment2d.cc

  \brief Segment library implementation

*/

#include "segment2d.hh"
#include "vector2d.hh"

/*! \brief Determine the intersection of two segments defined by their
    vertices

    \param[in] A0 1st vertex of the 1st segment
    \param[in] A1 2st vertex of the 1st segment
    \param[in] B0 1st vertex of the 2st segment
    \param[in] B1 2st vertex of the 2st segment
    \param[out] X intersection point. Underfined is return value is 
               false

    \return true if segments intersect, false otherwise

 */
bool segment2d_intersect ( const Point2d& A0, const Point2d& A1,
                           const Point2d& B0, const Point2d& B1,
                           Point2d& X )
{
  Vector2d a(A1-A0), b(B1-B0), c(B0-A0); // connection vectors

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
