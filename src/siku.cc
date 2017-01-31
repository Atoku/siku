/*! \file siku.cc
 *  \brief Main program
 */

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

#include <iostream>

extern "C"
{
#include <assert.h>
#include "shapefil.h"
#include <Python.h>
#undef tolower                  // stupid python defines tolower for
                                // all causing error on boost
                                // functions replacing further
                                // definitions
#include "config.h"
}

#include "siku.hh"
#include "options.hh"
#include "sikupy.hh"

#include "element.hh"

#include "globals.hh"           // all main variables

#include "modeltime.hh"         // model time

#include "forces_mass.hh"
#include "contact_force.hh"
#include "dynamics.hh"
#include "position.hh"
#include "coordinates.hh"

#include "highio.hh"
#include "monitoring.hh"
#include "mproperties.hh"

#include "contact_detect.hh"

////////////////
//#include  "nmc_reader.hh"
#include "vecfield.hh"

using namespace Coordinates;
//nullvec = vec3d( 0., 0., 0. );

//const char* coastline = "/Users/kulchits/Documents/center/active/siku/data/gshhg-shp-2.2.2/GSHHS_shp/c/GSHHS_c_L1.shp";

//--------------------------------------------------------------------
//! \brief main function
int
main ( int argc, char* argv[] )
{
  try{
  // Reading and populating program options
  Options options ( argc, argv );

  // Info
  if ( options.is_verbose () )
    std::cout << "Reading config: " << options.get_pythonfname () << std::endl;

  //// Coordinate transforms
  // Coordinates coords;

  // Variables!
  Globals siku;

  // I/O Interface
  Highio highio;

  // Loading the config file as a module and getting PyObject* to siku
  // namespace
  Sikupy sikupy ( options.get_pythonfname (), argc, argv );

  // Initializing all global variables from config file.
  sikupy.initialize ( siku );

//// Deprecated: loads with python
//  // If there is actually load file name - load from that file
//  if( siku.loadfile.length() )
//    highio.load( siku, siku.loadfile );

  if ( options.is_verbose () )
    std::cout << "End of reading config file" << std::endl;

  // Post-initialization of globals
  siku.post_init(sikupy);

  std::cout << "Post init done" << endl;
  
  // Precycle initialization:
  // Recovering mass, moments of inertia, other parameters if necessary
  mproperties ( siku );

  std::cout << "mproperties done" << endl;
  
  // python custom initializations
  sikupy.fcall_inits( siku );

  std::cout << "fcall inits done" << endl;
  
  // siku.time.print ();

  // yet it is here, but I think it should be somewhere in mproperties or close
  if( siku.ConDet.links.size() )
    siku.ConDet.freeze_links( siku );
  else if( siku.ConDet.inital_freeze )
    siku.ConDet.freeze( siku );

  cout<<"Elements: "<<siku.es.size()<<endl;

//  cout<<"siku.cont_force_model: "<<siku.cont_force_model<<endl;
//  cin.get();

  // Main Time Loop
  //while ( !siku.time.is_done () )
  do
    {
      // ------------------------- preactions ------------------------------

      double dt = siku.time.get_dt ();

      //      cout<<"\n Step: " << siku.time.get_n()<<endl;
      //cout<<"\r Step: " << siku.time.get_n()<<"\t";
      //cout.flush();
      //cout<<"dt: "<<dt<<endl;

      // --- pretimestep
      (void) sikupy.fcall_pretimestep ( siku );

      // --- Searching for interaction pairs
      siku.ConDet.detect( siku );

      // --- Broad Phase Contact Detection if necessary

      // ------------------------- physics ------------------------------

      // --- Mass Forces assignment (Drivers, Coriolis)
      forces_mass( siku );

      // --- Contact Forces assignment (Elements` interaction)
      contact_forces( siku );

      // --- Dynamics solution
      dynamics ( siku, dt );

      // --- Position update
      position ( siku, dt );

      // --- State update
      mproperties ( siku );

      // ------------------------- postactions ------------------------------

      // ---- Saving ---
      if ( siku.time.is_savetime () )
        {
          // why have this been marked as 'void'? It returns save/not status!
          //(void)
          int save_status = sikupy.fcall_presave ( siku );
          //no function = no action

          if( save_status == sikupy.FCALL_OK ) // odd mask processing coz OK=0
            highio.save( siku );

          siku.time.save_increment ();
        }

      // --- Monitoring functions
      monitoring ( siku, sikupy );

      // -- Diagnostics functions
      diagnosting ( siku, sikupy );

      // --- Cleaning of accumulating values etc.
      clean_props ( siku );

      // --- Concluding call back functions
      (void) sikupy.fcall_aftertimestep ( siku );

      // --- END OF LOOP ---
      siku.time.increment ();

      // ------------------------------------------------------------------
    }
  while ( !siku.time.is_done () );

  // finalizing
  sikupy.fcall_conclusions( siku );

  cout<<"\nDONE!\n";
  return 0;
  }
  catch( const char* c )
  {
      cout<<c<<endl;
  }
}
