/*
 * geom_types.hh
 *
 *  Created on: 22 марта 2016
 *      Author: gleb
 */

#ifndef GEOM_TYPES_HH_
#define GEOM_TYPES_HH_

// #include chain:
// gtypes -- vec3d - pnt3d - mat3d - segment2d - polyhedron3d -- geometry
//        \- vec2d - pnt2d - segment2d - polygon2d --         -/

#include <iostream>

// ====================== Implementation black magic ========================

  // removing degrees support to avoid warning message
  #define GLM_FORCE_RADIANS

  //TODO: DO implement this
  //typedef double greal;

  //! Main quaternion types
  #include <glm/gtc/quaternion.hpp>
  namespace Geometry
  {
    ////// temporally disabled to avoid ambiguities in H5 save/load
    //#ifdef SIKU_QUATF
    //typedef glm::fquat quat;
    //#else
    typedef glm::dquat quat;
    //#endif

    //! Streams printing
    inline std::ostream& operator<<( std::ostream& out, const quat& q )
    {
      out << q.w << " " << q.x << " " << q.y << " " << q.z;
      return out;
    }

  }

#endif /* GEOM_TYPES_HH_ */
