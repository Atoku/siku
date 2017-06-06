/*
 * Siku: Discrete element method sea-ice model: description.hh
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

/*!

  \file diagnostics.cc

  \brief Implementation of diagnostics class

*/

//#include "diagnostics.hh"

#ifdef AAA

void Diagnostics::run( Globals& siku, 
                       Sikupy& sikupy )
{
  if ( ! turned_on() ) return;

  run_wind( siku, sikupy );
}

void Diagnostics::run_wind( Globals& siku, 
                            Sikupy& sikupy )
{
  // all that need to be output
  for ( auto& dw: diag_wind )
    {
      cout << diag_wind.ifunc << endl;
      // vector<vec3d> data;

      // // Restore values
      // for ( auto& x: meshes[ dw.imesh ] )
      //   {
      //     vec3d u;
      //     wind.get_at_xy( x, &u );
      //     data.push_back( u );
      //   }

      // // Call callback from python
      // sikupy.fcall_diagnostics_vec3d( siku, dw.ifunc, data );
    }
}

#endif
