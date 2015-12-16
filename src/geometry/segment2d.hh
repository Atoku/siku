/*!

  \file segment.hh

  \brief Segment operation library

*/

#ifndef SEGMENT_HH
#define SEGMENT_HH

#include "point2d.hh"


/*!
  \brief finds the intersection of two segments

  \param[in] A0 first vertex of the first segment
  \param[in] A1 second vertex of the first segment
  \param[in] B0 first vertex of the second segment
  \param[in] B1 second vertex of the second segment
  \param[out] X the point where the lines intersect
  
  \return True on intersection, False otherwise

 */
bool segment_intersect ( const Point2d& A0, const Point2d& A1,
                         const Point2d& B0, const Point2d& B1,
                         Point2d& X );


#endif      /* SEGMENT_HH */
