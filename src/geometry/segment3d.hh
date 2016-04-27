/*
 * segment3d.hh
 *
 *  Created on: 22 марта 2016
 *      Author: gleb
 */

#ifndef SEGMENT3D_HH_
#define SEGMENT3D_HH_

#include "matrix3d.hh"
// #include chain:
// gtypes -- vec3d - pnt3d - mat3d - segment3d - polyhedron3d -- geometry
//        \- vec2d - pnt2d - segment2d - polygon2d --         -/


namespace Geometry
{

  //! \brief checks if line segment and a plane intersect.
  //! \param[in] v1 - first point of segment
  //! \param[in] v2 - second point of segment
  //! \param[in] p - any point on plane (aka transition)
  //! \param[in] norm - plane normal (aka orientation)
  //! \param[out] X - intersection point (if intersect)
  //! \return true if intersect, false if not
  bool segment3d_surf_inter( const pnt3d& v1, const pnt3d& v2,
                             const pnt3d& p, const vec3d& norm, pnt3d& X );

}

#endif /* SEGMENT3D_HH_ */
