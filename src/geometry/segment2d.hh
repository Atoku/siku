/*!

  \file segment.hh

  \brief Segment operation library

*/

#ifndef SEGMENT_HH
#define SEGMENT_HH

#include "point2d.hh"

// TODO: deprecate dis mess
//! \brief checks if two line segments intersect.
//! \param[in] a1 first point of segment 'a'
//! \param[in] a2 second point of segment 'a'
//! \param[in] b1 first point of segment 'b'
//! \param[in] b2 second point of segment 'b'
//! \param[out] X - point where the segments intersect (if they do)
//! \return true if segments intersect, false if not
bool line_seg_inter( const pnt2d& a1, const pnt2d& a2,
                      const pnt2d& b1, const pnt2d& b2, pnt2d& X );

/*!
  \brief finds the intersection of two segments

  \param[in] A0 first vertex of the first segment
  \param[in] A1 second vertex of the first segment
  \param[in] B0 first vertex of the second segment
  \param[in] B1 second vertex of the second segment
  \param[out] X the point where the lines intersect (underfined is not
  intersect)
  
  \return True on intersection, False otherwise

 */
bool segment2d_intersect ( const pnt2d& A0, const pnt2d& A1,
                           const pnt2d& B0, const pnt2d& B1,
                           pnt2d& X );

#endif      /* SEGMENT_HH */
