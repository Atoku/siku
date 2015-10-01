/*! \file siku.hh
 *  \brief Default values and some basic definitions
 */

/*
 * Siku: Discrete element method sea-ice model: siku.cc
 *
 * Copyright (C) 2013 UAF. Author: Anton Kulchitsky
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

#ifndef SIKU_HH
#define SIKU_HH

//! Default name for Lua configuration file
#define SIKU_DEFAULT_PYTHON "sikucfg.py"

//===== GLM =====
// removing degrees support to avoid warning message
#define GLM_FORCE_RADIANS

//! Main vector types
#include <glm/vec3.hpp>
#ifdef SIKU_VECF                // float vectors
typedef glm::fvec3 vec3d;
#else
typedef glm::dvec3 vec3d;
#endif

//! Main quaternion types
#include <glm/gtc/quaternion.hpp>
#ifdef SIKU_QUATF
typedef glm::fquat quat;
#else
typedef glm::dquat quat;
#endif

//! Main matrix types
#include <glm/mat3x3.hpp>
#ifdef SIKU_QUATF
typedef glm::fmat3 mat3d;
#else
typedef glm::dmat3 mat3d;
#endif

//////////// FOR TESTING /////////////
#include <iostream>

void outVec( const vec3d& vec);

///////////////////////////

//! \brief inlined dot product for vec3d
inline double dot_prod ( const vec3d& v1, const vec3d& v2 )
{
  return v1.x*v2.x + v1.y*v2.y +v1.z*v2.z;
}

//! \brief inlined method for getting square modulus of vecs` difference
inline double sq_dist( const vec3d& v1, const vec3d& v2 )
{
  return dot_prod( v2 - v1, v2 - v1 );
}

inline double square_( const double& x) { return x*x; }

inline double vec_len( const vec3d& vec)
{
  return sqrt( vec.x*vec.x + vec.y*vec.y + vec.z*vec.z );
}

#endif  // SIKU_HH
