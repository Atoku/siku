/*
 * Siku: Discrete element method sea-ice model: element.hh
 *       Ice Element class: ice element representation and methods
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

/*! \file element.hh
 *  \brief class element for representing ice elements
 */

#ifndef ELEMENT_HH
#define ELEMENT_HH

#include <vector>
#include <string>
using namespace std;

#include "siku.hh"

//! \brief Ice element class: representation for a single ice element
class Element
{
public:

  //! \brief flag state for free body element
  static const unsigned int F_FREE   {0x1};
  //! \brief flag state for steady body element, that remains same v
  //! and w as at the beginning
  static const unsigned int F_STEADY {0x2};
  //! \brief flag state for static body element with always v=0, w=0
  static const unsigned int F_STATIC {0x4};
  //! \brief flag state for all movement states (filtering all other flags)
  static const unsigned int F_MOVE_FLAG {0x7};
  //! \brief flag state for controlled object that looks for control
  //! function to determine v and w
  static const unsigned int F_CONTROLLED {0x8};
  //! \brief flag state for monitored object that exchanges
  //! information with python script
  static const unsigned int F_MONITORED  {0x10};

  // --------------- Not changing handling parameters ----------------

  unsigned int flag;            //!< state flag
//  char* monitor {nullptr};         //!< monitor function name (0-term)
//  char* control {nullptr};         //!< control function name (0-term)
  //std::string monitor;         //!< monitor function name
  //std::string control;         //!< control function name
  size_t mon_ind { 0 };               //!< monitor function index
  size_t con_ind { 0 };               //!< control function index

  // --------------- Not changing state parameters -------------------
  
  double i;                     //!< I/m, geometical moment of inertia
  double A;                     //!< m^2, area of polygon
  vector<double> gh;            //!< g(h) thickness distribution 
  vector<vec3d> P;              //!< 1, local unit frame coords of
                                //! vertices
  double sbb_rmin;              //!< bounding sphere minimum radius
  size_t imat;                  //!< material index
  size_t igroup;                //!< group index
  
  // --------------- Changing parameters -----------------------------

  //! \brief "Pendulum" orientation: defines the position of the ice
  //! element on a sphere
  quat q;

  vec3d Glob;                   //!< global position in (x, y, z)
  vec3d V;                      //!< local surface velocity (x, y, 0)

  double m;                     //!< kg, mass
  double I;                     //!< moment of inertia

  vec3d W;                      //!< 1/m, angular velocity in local
                                //!coord.
  vec3d F;                      //!< N, net force vector in local
                                //!frame
  double N {0};                 //!< N*m, torque value in local frame

  bool ERRORED{ false };        //!< flag for runtime error detection

  // ------------------- METHODS: -------------------------------------

  //! Check if point (given inglobal x,y,z ) is inside the element
  bool contains( const vec3d& p );

  //! Destructor takes care about monitor and control functions
  ~Element()
  {
    //if ( monitor ) delete monitor;
    //if ( control ) delete control;
  }
};

#endif
