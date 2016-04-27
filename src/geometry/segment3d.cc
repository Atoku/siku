/*
 * segment3d.cc
 *
 *  Created on: 22 марта 2016
 *      Author: gleb
 */

#include "segment3d.hh"

namespace Geometry
{

  bool segment3d_surf_inter( const pnt3d& v1, const pnt3d& v2,
                             const pnt3d& p, const vec3d& norm, pnt3d& X )
  {
    // relative positions
    vec3d seg = v2 - v1,
          pv1 = v1 - p,
          pv2 = v2 - p;

    // several projections
    double seg_d_norm = dot( seg, norm ),
           pv1_d_norm = dot( pv1, norm ),
           pv2_d_norm = dot( pv2, norm );

    if( seg_d_norm  == 0. )  // <-segment is parallel to the plane
      {
        if( pv2_d_norm == 0. ) // <-lies within it
          {
            X = (v1 + v2) * 0.5;
            return true;
          }
        else
          return false;
      }

    // different signs = intersect, zero = vertex touch
    if( pv1_d_norm * pv2_d_norm <= 0. )
      {
        X = v1 + seg * ( -pv1_d_norm / seg_d_norm );
        return true;
      }

    return false;
  }

}
