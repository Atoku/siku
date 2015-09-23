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

//---------------------------------------------------------------------

void ContactDetector::find_pairs( Globals& siku )
{
  cont.clear();

  for ( size_t i = 0; i < siku.es.size () - 1; ++i )
    {
      for ( size_t j = i + 1; j < siku.es.size (); ++j )
        {
          if ( vec_len ( siku.es[i].Glob - siku.es[j].Glob ) <
              ( siku.es[i].sbb_rmin + siku.es[j].sbb_rmin ) )
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
  }
}
