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

/*!

 \file nmc_reader.cc

 \brief NMC_* classes

 */

#include "nmc_reader.hh"


//////////////for test
#include <iostream>
using namespace std;

//----------------------------------------------------------------------------
//--------------------------- NMC Vec Field ----------------------------------
//----------------------------------------------------------------------------

vec3d
NMCVecfield::get_vec ( const int& lat_i, const int& lon_i )
{
  try
    {
      return grid.at ( lat_i % grid.size() ).at ( lon_i % grid[0].size() );
    }
  catch ( const std::out_of_range& oor )
    {
      /*
       * TODO: exception handling must be added
       */
      cout<<lat_i<<" "<<lon_i<<" out_of_range get_vec_i!\n";
      throw( oor );
      //return nullptr;
    }
}

//-------------------------------------------------------------------

vec3d
NMCVecfield::get_vec ( const double& lat, const double& lon )
{
  try
    {
      return grid.at ( lat_indexer.at ( lat ) ).at ( lon_indexer.at ( lon ) );
    }
  catch ( const std::out_of_range& oor )
    {
      /*
       * TODO: exception handling must be added
       */
      cout<<"out_of_range get_vec_d!\n";
      throw( oor );
      //return nullptr;
    }
}

//-------------------------------------------------------------------

NMCVecfield::GridNode
NMCVecfield::get_node ( const size_t& lat_i, const size_t& lon_i )
{
  try
    {
      return NMCVecfield::GridNode ( lat_valuator.at ( lat_i ),
                                     lon_valuator.at ( lon_i ),
                                     grid.at ( lat_i ).at ( lon_i ) );
    }
  catch ( const std::out_of_range& oor )
    {
      /*
       * TODO: exception handling must be added
       */
      cout<<"out_of_range get_node_i!\n";
      throw( oor );
      //return nullptr;
    }
}

//-------------------------------------------------------------------

NMCVecfield::GridNode
NMCVecfield::get_node ( const double& lat, const double& lon )
{
  try
    {
      return NMCVecfield::GridNode ( lat, lon,
          grid.at ( lat_indexer.at ( lat ) ).at ( lon_indexer.at ( lon ) ) );
    }
  catch ( const std::out_of_range& oor )
    {
      /*
       * TODO: exception handling must be added
       */
      cout<<"out_of_range get_node_d!\n";
      throw( oor );
      //return nullptr;
    }
}

//-------------------------------------------------------------------

void
NMCVecfield::set_vec ( const vec3d& value, const size_t& lat_i,
                       const size_t& lon_i )
{
  try
    {
      grid.at ( lat_i ).at ( lon_i ) = value;
    }
  catch ( const std::out_of_range& oor )
    {
      /*
       * TODO: exception handling must be added
       */
      cout<<"out_of_range set_vec_i!\n";
      //error: "illegal index"
      throw( oor );
    }
}

//-------------------------------------------------------------------

void
NMCVecfield::set_vec ( const vec3d& value, const double& lat,
                       const double& lon )
{
  try
    {
      grid.at ( lat_indexer.at ( lat ) ).at ( lon_indexer.at ( lon ) ) = value;
    }
  catch ( const std::out_of_range& oor )
    {
      /*
       * TODO: exception handling must be added
       */
      cout<<"out_of_range set_vec_d!\n";
      //error: "bad argument: no such coordinates"
      throw( oor );
    }
}

//-------------------------------------------------------------------

void
NMCVecfield::set_node ( const GridNode& GN, const size_t& lat_i,
                        const size_t& lon_i )
{
  try
    {
      grid.at ( lat_i ).at ( lon_i ) = GN.value;
      lat_indexer[GN.lat] = lat_i;
      lon_indexer[GN.lon] = lon_i;
      lat_valuator[lat_i] = GN.lat;
      lon_valuator[lon_i] = GN.lon;
    }
  catch ( const std::out_of_range& oor )
    {
      /*
       * TODO: exception handling must be added
       */
      cout<<"out_of_range set_node!\n";
      //error: "illegal index"
      throw( oor );
    }
}

//-------------------------------------------------------------------

void
NMCVecfield::init_grid ( const size_t& lat_s, const size_t& lon_s )
{
  clear ();
  grid.resize ( lat_s, std::vector < vec3d > ( lon_s ) );
  //lat_step = la_step;
  //lon_step = lo_step;
}

//-------------------------------------------------------------------

void
NMCVecfield::clear ()
{
  for ( size_t i = 0; i < grid.size (); ++i )
    grid[i].clear ();
  grid.clear ();
  lat_indexer.clear ();
  lon_indexer.clear ();
  lat_valuator.clear ();
  lon_valuator.clear ();
}

