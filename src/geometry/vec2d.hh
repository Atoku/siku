/*!

  \file vec2d.hh

  \brief 2D vector type class

*/

/*
 * Siku: Discrete element method sea-ice model: siku.cc
 *
 * Copyright (C) 2015-2016 UAF.
 * Authors: Anton Kulchitsky, Gleb Velikhovskiy
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

#ifndef VEC2D_HH
#define VEC2D_HH

//#include <vector>
#include <iostream>
using namespace std;

extern "C" {
#include <string.h>
#include <math.h>
#include <stdio.h>
#include <assert.h>
}

namespace Geometry
{

    struct vec2d
    {
    //private:
    public:
      double x;
      double y;

    public:

      // ------------------ simple constructors & destructor -------------

      //vec2d() = default; // POD-style, discuss this and clean.

//      //! default constructor
//      vec2d( const double& X = 0., const double& Y = 0. ) :
//        x( X ), y( Y ) {}
//
//      //! simple copy
//      vec2d( const vec2d& o )
//      {
//        memcpy( &x, &o.x, 2 * sizeof(double) );
//      }

      //! default destructor
      ~vec2d() = default;

      // ------------------------ assignments and access -----------------

      // basic access
      inline double getx() const { return x; }
      inline double gety() const { return y; }

      inline void setx( const double& X ) { x = X; }
      inline void sety( const double& Y ) { y = Y; }

      // ------------------------ assignment ----------------------------

      // simple assignment operator
      inline vec2d& operator= ( const vec2d& V )
      {
        memcpy( &x, &V.x, 2 * sizeof(V.x) );
        return *this;
      }

      //------------------------------------------------------------------
      // Basic operations
      //------------------------------------------------------------------

      //! unary minus = inversion
      inline vec2d operator - ()
      {
        return { -x, -y };
      }

      //! promotion
      inline vec2d operator + ()
      {
        return { x, y };
      }

      //! addition by a vector
      inline vec2d& operator += ( const vec2d& V )
      {
        x += V.x;
        y += V.y;
        return *this;
      }

      //! subtraction by a vector
      inline vec2d& operator -= ( const vec2d& V )
      {
        x -= V.x;
        y -= V.y;
        return *this;
      }

      //! multiplication by a number
      inline vec2d& operator *= ( const double& d )
      {
        x *= d;
        y *= d;
        return *this;
      }

      //! division by a number
      inline vec2d& operator /= ( const double& d )
      {
        x /= d;
        y /= d;
        return *this;
      }

      inline vec2d operator + ( const vec2d& V ) const
      {
        return { x + V.x, y + V.y };
      }

      inline vec2d operator - ( const vec2d& V ) const
      {
        return { x - V.x, y - V.y };
      }

      inline vec2d operator * ( const double& d ) const
      {
        return { x * d, y * d };
      }

      //! division by number (TODO: we don't need it) Why? Renormalization, ort
      // retrieving and mean value calculation depends on this division. If
      // multiply by reversed number - some fails may occur.
      inline vec2d operator / ( const double& d ) const
      {
        return { x / d, y / d };
      }

      //! scalar (dot) product
      inline double operator * ( const vec2d& V ) const
      {
       return x * V.x + y * V.y;
      }

      // ------------------------------ comparison------------------------

      inline bool operator== (const vec2d &V) const
      {
        return x == V.x && y == V.y;
      }

      inline bool operator!= (const vec2d &V) const
      {
        return x != V.x || y != V.y;
      }

      inline operator bool () const { return x || y; }

      // --------------------------- various methods ---------------------

      //! absolute value
      inline double abs() const
      {
        return sqrt( x * x + y * y );
      }

      //! squared absolute value
      inline double abs2() const { return x * x + y * y; }

      //! renorm vector by optional given length (1 by default) and
      //! return it
      inline vec2d& renorm( double d = 1. )
      {
        d /= abs();
        return ( *this *= d );
      }

      // return ort of vector
      inline vec2d ort() const { return ( *this / abs() ); }

      // dot product
      inline double dot( const vec2d& V ) const
      {
        return ( *this * V );
      }

      //cross product
      inline double cross( const vec2d& V ) const
      {
        return x * V.y - y * V.x;
      }

      //---------------------------------------------------------------------



      //---------------------------------------------------------------------
      //! pretty print
      inline void print() const
      {
        cout << x << '\t' <<  y << endl;
      }

    };

    //-----------------------------------------------------------------------
    // ~~~~~~~~~~~~~~~~~~~~~ Exterior functionality ~~~~~~~~~~~~~~~~~~~~~~~~~
    //-----------------------------------------------------------------------

    inline vec2d operator * ( const double& d, const vec2d& V )
    {
      return V * d;
    }

    inline double abs( const vec2d& V ) { return V.abs(); }
    inline double abs2( const vec2d& V ) { return V.abs2(); }

    inline vec2d& renorm( vec2d& V, const double& scale = 1. )
    {
      return V.renorm( scale );
    }

    inline double dot( const vec2d& V1, const vec2d& V2 )
    {
      return V1.dot( V2 );
    }

    inline double cross ( const vec2d& V1, const vec2d& V2 )
    {
      return V1.cross( V2 );
    }

    inline vec2d ort ( const vec2d& V ) { return V.ort(); }

    inline void print( const vec2d& v )  //experimental
    {
      v.print();
    }

    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Utils ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    //! Pretty print with standard %f format
    #define vec2d_print( v ) \
      cout << #v << ":  "; (v).print();

    //! Pretty pring with custom format
    //! Sample:   Vector2d_printf( vec, "%2.2f" );
    #define vec2d_printf( v, f ) \
      printf( #v " = ( " f", " f" )\n", (v).getx(), (v).gety() )

}

#endif  /* VECTOR2D_HH */
