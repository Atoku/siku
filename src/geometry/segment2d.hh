/*!

  \file segment.hh

  \brief Segment operation library

*/

#ifndef SEGMENT_HH
#define SEGMENT_HH

#include "pnt2d.hh"

namespace Geometry
{

  // TODO: deprecate dis mess
  //! \brief checks if two line segments intersect.
  //! \param[in] a1 first point of segment 'a'
  //! \param[in] a2 second point of segment 'a'
  //! \param[in] b1 first point of segment 'b'
  //! \param[in] b2 second point of segment 'b'
  //! \param[out] X - point where the segments intersect (if they do)
  //! \return true if segments intersect, false if not
  bool segment2d_intersect( const pnt2d& a1, const pnt2d& a2,
                       const pnt2d& b1, const pnt2d& b2, pnt2d& X );


}

#endif      /* SEGMENT_HH */
