/*
 * Siku: Discrete element method sea-ice model: siku.cc
 *
 * Copyright (C) 2013-2014 UAF. Author: Anton Kulchitsky
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

#include "globals.hh"

#include "contact_detect.hh"
using namespace Coordinates;

// we need some sorting!
#include <algorithm>

//---------------------------------------------------------------------

// For sorting watch https://s-media-cache-ak0.pinimg.com/originals/5f/fc/42/5ffc4224b938d1fb0abee887e4add84b.jpg
// Yet simple 'sort' from <algorithm> is used.

inline bool elements_x_compare( const Element& e1, const Element& e2 )
{
  /*
   * TODO: i think we should calculate xmin and xmax on update phase and store
   * inside each element, rather then calculate several times in one check!
   */
  return (e1.Glob.x - e1.sbb_rmin) < (e2.Glob.x - e2.sbb_rmin);
}

void ContactDetector::sweep_n_prune( Globals& siku )
{
  // sorting
  std::sort( siku.es.begin(), siku.es.end(), elements_x_compare );

  // contact search
  cont.clear();

  for ( size_t i = 0; i < siku.es.size () - 1; ++i )
    {
      for ( size_t j = i + 1; j < siku.es.size (); ++j )
        {
          if ( sq_dist( siku.es[i].Glob, siku.es[j].Glob ) <
              square_( siku.es[i].sbb_rmin + siku.es[j].sbb_rmin ) )
            {
              cont.push_back ( Contact ( i, j, siku.time.get_n () ) );
              cout << "SWEEP PAIR " << i << "-" << j << "  step: "
                  << siku.time.get_n()<<endl;
            }
          if ( (siku.es[i].Glob.x + siku.es[i].sbb_rmin) <
              (siku.es[j].Glob.x - siku.es[j].sbb_rmin) )

              break;

        }
    }
}

//---------------------------------------------------------------------

void ContactDetector::find_pairs( Globals& siku )
{
  cont.clear();

  for ( size_t i = 0; i < siku.es.size () - 1; ++i )
    {
      for ( size_t j = i + 1; j < siku.es.size (); ++j )
        {
          if ( sq_dist( siku.es[i].Glob, siku.es[j].Glob ) <
              square_( siku.es[i].sbb_rmin + siku.es[j].sbb_rmin ) )
            {
              cont.push_back ( Contact ( i, j, siku.time.get_n () ) );
              cout << "PAIR " << i << "-" << j << "  step: "
                  << siku.time.get_n()<<endl;
            }
        }
    }
}

//---------------------------------------------------------------------

void  ContactDetector::detect( Globals& siku )
{
  switch( det_meth )
  {
    case CONTACTS_N2:
      find_pairs( siku );
      break;
    case SWEEP_N_PRUNE:
      sweep_n_prune( siku );
      break;
  }
}
