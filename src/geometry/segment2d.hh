/*!

  \file segment.hh

  \brief Segment operation library

*/

#ifndef SEGMENT_HH
#define SEGMENT_HH

#include "pnt2d.hh"

namespace Geometry
{

  //! \brief checks if two line segments intersect.
  //! \param[in] a1 first point of segment 'a'
  //! \param[in] a2 second point of segment 'a'
  //! \param[in] b1 first point of segment 'b'
  //! \param[in] b2 second point of segment 'b'
  //! \param[out] X - point where the segments intersect (if they do)
  //! \return true if segments intersect, false if not
  bool segment2d_intersect( const pnt2d& a1, const pnt2d& a2,
                       const pnt2d& b1, const pnt2d& b2, pnt2d& X );

  //! \brief calculates the distance between two segments
  //! \param[in] a1 first point of segment 'a'
  //! \param[in] a2 second point of segment 'a'
  //! \param[in] b1 first point of segment 'b'
  //! \param[in] b2 second point of segment 'b'
  //! \return 0 if segments intersect, distance otherwise
  double segment2d_distance( const pnt2d& a1, const pnt2d& a2,
                             const pnt2d& b1, const pnt2d& b2 );


  //! \brief checks if a line intersects a segment
  //! \param[in] p - any point on line
  //! \param[in] n - normal (perpendicular) direction to line
  //! \param[in] v1, v2 - vertices of segment
  //! \param[out] X - intersection point (if intersect)
  //! \return true if intersect, false if not
  bool segment2d_line_inter( const pnt2d& p, const vec2d& n,
                             const pnt2d& v1, const pnt2d& v2, pnt2d& X );

}

#endif      /* SEGMENT_HH */
