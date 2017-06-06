/*
 * Siku: Discrete element method sea-ice model: description.hh
 *
 * Copyright (C) UAF
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */

/*
 * geom_types.hh
 *
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
