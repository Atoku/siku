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

// comparator for sorting algorithm
inline bool elements_x_compare( const Element& e1, const Element& e2 )
{
  /*
   * TODO: i think we should calculate xmin and xmax on update phase and store
   * inside each element, rather then calculate several times in one check!
   */
  return (e1.Glob.x - e1.sbb_rmin) < (e2.Glob.x - e2.sbb_rmin);
}

// pointer comparator for sorting algorithm
inline bool el_pointers_x_compare( Element* pe1, Element* pe2 )
{
  return elements_x_compare( *pe1, * pe2 );
  //return (pe1->Glob.x - pe1->sbb_rmin) < (pe2->Glob.x - pe2->sbb_rmin);
}

void ContactDetector::sweep_n_prune( Globals& siku )
{
  // preparing additional vector:
  // setting pointers in supporting vector (for sortings, etc)
  siku.pes.clear();
  for( size_t i = 0; i < siku.es.size(); ++i )
    siku.pes.push_back( &siku.es[i] );

  // sorting
  std::sort( siku.pes.begin(), siku.pes.end(), el_pointers_x_compare );

  // contact search
  cont.clear();

  for ( size_t i = 0; i < siku.pes.size () - 1; ++i )
    {
      for ( size_t j = i + 1; j < siku.pes.size (); ++j )
        {
          if ( sq_dist( siku.pes[i]->Glob, siku.pes[j]->Glob ) <
              square_( siku.pes[i]->sbb_rmin + siku.pes[j]->sbb_rmin ) )
            {
              cont.push_back ( Contact (
                  siku.pes[i]->id, siku.pes[j]->id, siku.time.get_n () ) );
//              cout << "SWEEP PAIR " << i << "-" << j << "  step: "
//                  << siku.time.get_n()<<endl;
            }
          if ( (siku.pes[i]->Glob.x + siku.pes[i]->sbb_rmin) <
              (siku.pes[j]->Glob.x - siku.pes[j]->sbb_rmin) )

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
//              cout << "PAIR " << i << "-" << j << "  step: "
//                  << siku.time.get_n()<<endl;
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
