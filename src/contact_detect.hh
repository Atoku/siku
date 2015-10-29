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

// predeclaration due to circled includes
struct Globals;

// contact detection methods classification
enum : unsigned long
{
  CONTACTS_N2 = 0,
  SWEEP_N_PRUNE = 1
};

//! \brief Functions class: provides several methods for contact detection,
//! storage and history access
class ContactDetector
{
public:
  //! \brief Inner structure for holding interaction pairs metadata
  struct Contact
  {
    size_t i1 {0};
    size_t i2 {0};
    int step{0};
    Contact(){}
    Contact(const size_t& i1_, const size_t& i2_, const int& s ): step(s)
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
  };

  //! \brief Method specifier (ye, i know what 'meth' means...)
  unsigned long det_meth{ CONTACTS_N2 };

  // contacts pool
  std::vector < Contact > cont;

  //! \brief Method for calling contacts detection
  void detect( Globals& siku );

private:
  //! \brief simple method for contacts detection. N^2 complexity.
  void find_pairs( Globals& siku );

  //! \brief sweep and prune method for contacts detection.
  void sweep_n_prune( Globals& siku );

};

#endif
