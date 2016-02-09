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

#ifndef INTERACTION_HH
#define INTERACTION_HH

#include "siku.hh"
#include "element.hh"
#include "coordinates.hh"

#include <vector>

// predeclaration due to circled includes (yes, we have spaghetti-code)
struct Globals;

// contact detection methods classification
enum : unsigned long
{
  CONTACTS_N2 = 0,
  SWEEP_N_PRUNE = 1
};

// enum of methods for contact detection frequency calculation
enum : unsigned long
{
  ALWAYS = 0,
  BY_TICKS = 2,
  BY_SECONDS = 3,
  BY_SPEED = 4
};

// contact state flag (outside of classes for fast access)
enum ContType: unsigned long
{
  NONE = 0,
  COLLISION = 1,
  JOINT = 2  // aka coalesced
};

//! \brief Functions class: provides several methods for contact detection,
//! storage and history access
class ContactDetector
{
public:
  //! \brief Inner structure for holding interaction pairs metadata
  struct Contact
  {
    ContType type { NONE };
    size_t i1 { 0 };
    size_t i2 { 0 };
    int generation{ 0 };  //
    int step{ -1 };  // step when was created. -1 marks default object
    double durability{ 1. };  // must be discussed
    double area{ 0. };  // area of contact
    Geometry::vec2d p1{}, p2{};  // positions of joint center in local
                                 // coords of contacting polygons

    Contact(){}
    Contact(const size_t& i1_, const size_t& i2_, const int& s,
            const ContType& ct = ContType::NONE ): step(s)
    {
      // first element in contact should remain first in list (lower id)
      if( i1_ < i2_)
        {
          i1 = i1_;
          i2 = i2_;
        }
      else
        {
          i1 = i2_;
          i2 = i1_;
        }
    }

    inline bool operator < ( const Contact& c ) const
    {
      return i1 < c.i1 || (i1 == c.i1 && i2 < c.i2);
    }
  };

  //! \brief Method specifier (ye, i know what 'meth' means...)
  unsigned long det_meth{ CONTACTS_N2 };

  //! \brief detection frequency type
  unsigned long det_freq_t{ ALWAYS };

  //! \brief contact detection XX value (XX may be period, speed, etc)
  double det_value;

private:
  //! \brief util value to store previous 'det_value'
  double det_last{ 0. };
public:

  // contacts pool
  std::vector < Contact > cont;

  //------------------------------- methods ----------------------------------

  //! \brief Method for calling contacts detection
  void detect( Globals& siku );

  //! \brief Method for generating 'joint' contacts - hard frozen connections.
  //! Second optional argument - tolerance of ice elements size. It is actually
  //! a scaling factor for temporal resizing for overlap detection.
  void freeze( Globals& siku, double tolerance = 0.01 );

private:
  //! \brief method to check if it is time to update contacts
  bool is_detect_time( Globals& siku );

  //! \brief simple method for contacts detection. N^2 complexity.
  void find_pairs( Globals& siku );

  //! \brief sweep and prune method for contacts detection.
  void sweep_n_prune( Globals& siku );

  //! \brief smart cleaning of contacts list. 'Frozen' (coalesced) contacts
  //! remain untouched until destroyed, other are renewed at each step
  void clear();

};

#endif
