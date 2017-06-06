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

/*! \file siku.hh
 *  \brief Default values and some basic definitions
 */

#ifndef SIKU_HH
#define SIKU_HH

#ifdef SIKU_FP_CHECK  /* files included only if FP_CHECK */

#if HAVE_FENV_H
#include <fenv.h> /*  adding NaN check for Linux */
//#pragma STDC FENV_ACCESS ON
#endif

#if HAVE_XMMINTRIN_H
#include <xmmintrin.h> /*  adding NaN check for SSE and MacOS */
#endif

#endif /* SIKU_FP_CHECK */

//! Max material layers amount
#ifdef SIKU_MAX_THICK
static const unsigned int MAT_LAY_AMO = SIKU_MAX_THICK; // WTF size_t does not work?!
#else
static const unsigned int MAT_LAY_AMO = 10; // WTF size_t does not work?!
#endif

//! Default name for Lua configuration file
#define SIKU_DEFAULT_PYTHON "sikucfg.py"

// geometry implementations
//#define SIKU_3D_NOT_GLM

#include "geometry/geometry.hh"

// TODO: clear this
typedef Geometry::vec2d vec2d;

using namespace Geometry;

////////////////////////// OBJECT VERIFICATION
//TODO: Delete or advance this macro - it spends unnecessary time!

inline bool _verify( const bool& b )   { return b; }
inline bool _verify( const int& i )    { return i; }
inline bool _verify( const double& d ) { return std::isfinite( d ); }
inline bool _verify( const vec2d& v )  { return !NaN( v ); }
inline bool _verify( const vec3d& v )  { return !NaN( v ); }
inline bool _verify( const quat& q )   { return !NaN( q ); }

#define VERIFY( _EXP, _MSG ) \
  if( ! _verify( (_EXP) ) ) \
    { \
      cout << "'" << (_EXP) << "' in: " << #_EXP << " (" << _MSG << ")" << endl; \
      cin.get(); \
    }

////////////////////////// \OBJECT VERIFICATION

#endif  // SIKU_HH
