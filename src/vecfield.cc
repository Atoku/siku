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

  \file vecfield.cc

  \brief Implementation of vector field class.

*/

#include "vecfield.hh"
#include "errors.hh"
#include "sikupy.hh"

using namespace Coordinates;

//---------------------------------------------------------------------
//---------------- UTIL FUNCTIONS FOR INTERPOLATION -------------------
//---------------------------------------------------------------------

inline vec3d proport( const vec3d& d1, const vec3d& d2, const double& t )
{
//  return vec3d( d1.x + (d2.x - d1.x)*t,
//                d1.y + (d2.y - d1.y)*t,
//                d1.z + (d2.z - d1.z)*t );
  return d1 + ( d2 - d1 )*t;
}

inline double fastNormLon360(double lon)
{
  //slow accurate variant - change 'if' with 'while'
  if( lon >= 2.*M_PI ) lon -= 2.*M_PI;
  if( lon < 0 ) lon += 2.*M_PI;
  return lon;
}
inline double fastNormLat180(double lat)
{
  //slow accurate variant - change 'if' with 'while'
  if( lat >= M_PI ) lat -= M_PI;
  if( lat < 0 ) lat += M_PI;
  return lat;
}

//---------------------------------------------------------------------
//---------------------------------------------------------------------
//---------------------------------------------------------------------

Vecfield::Vecfield(const Source_Type& SOURCE_TYPE,
                   const double gslo, const double gsla ) :
FIELD_SOURCE_TYPE( SOURCE_TYPE )
{
  mode = MODE_VEC_STD_FIELD1;
  if( FIELD_SOURCE_TYPE == NMC )  NMCVec = new NMCVecfield;
 // cout<<NMCVec<<" Vecfield!!!!! "<<SOURCE_TYPE<<endl;
}

//---------------------------------------------------------------------

Vecfield::Vecfield()
{
  //NMCVec = new NMCVecfield;
  mode = MODE_VEC_STD_FIELD1;
}

//---------------------------------------------------------------------

void Vecfield::init ( const Source_Type& SOURCE_TYPE,
                      const double gslo, const double gsla  )
{
  mode = MODE_VEC_STD_FIELD1;
  FIELD_SOURCE_TYPE = SOURCE_TYPE;

  if( FIELD_SOURCE_TYPE == NMC )  NMCVec = new NMCVecfield;

}

//---------------------------------------------------------------------

Vecfield::~Vecfield()
{
  if( NMCVec ) delete NMCVec; // safe deletion
}

//---------------------------------------------------------------------

void Vecfield::get_at_xy( const vec3d& x, vec3d* pv )
{
  switch ( mode )
    {
      case MODE_VEC_STD_FIELD1: 
        field1( x, pv );
        break;

      default:
        fatal( 1, "Not standard wind field is not implemented yet" );
        break;
    }
}

//---------------------------------------------------------------------

vec3d Vecfield::get_at_lat_lon_rad( double lat,  double lon )
{
  //draft: the simplest interpolation

  // just in case...
  if( FIELD_SOURCE_TYPE == NONE && !NMCVec )  return {0., 0., 0.};

 // inner testing
 if( FIELD_SOURCE_TYPE == TEST )
   {
     return Coordinates::geo_to_cart_surf_velo ( lat, lon, -10., 1. );
     //return Coordinates::geo_to_cart_surf_velo( lat lon, 10, 0 );
   }

 // input params check
 if( !std::isfinite(lat) || !std::isfinite(lon) )
   {
     cout<<"\nERROR: NaN lat or/and lon!\n"<<lat<<" "<<lon<<"\n\n";
     fatal( 1, "interpolation args are NaN of infinite!");
   }

  double mo = deg_to_rad( NMCVec->get_min_lon() ),
         Mo = deg_to_rad( NMCVec->get_max_lon() ),
         ma = deg_to_rad( NMCVec->get_min_lat() ),
         Ma = deg_to_rad( NMCVec->get_max_lat() );

 // normalizing latitude in range [ -Pi/2, Pi/2],
 // longitude in range [0, 2Pi] (for proper indexing)
 lat = norm_lat ( lat );// + M_PI/2.;
 lon = norm_lon ( lon );

//  // Region check. If outside - return zero-vector as nodata
  if( (lat > Ma) || (lat < ma) )        return {}; // out of grid
  else
    if( lon < mo )
      if( (lon + 2.* M_PI) > Mo )   return {}; // out of grid
      else                          lon += 2.* M_PI; // continue calculation
    else
      if( lon > Mo )                return {}; // out of grid

 // refreshing grid sizes
 lon_size = NMCVec->get_lon_size ();
 lat_size = NMCVec->get_lat_size ();

 // refreshing grid steps
  grid_step_lon = (Mo - mo) / (lon_size - 1);
  grid_step_lat = (Ma - ma) / (lat_size - 1);

 // calculating current cell indexes
 int lat_ind = norm_lat_ind( (((double)lat - ma) / grid_step_lat) );
 if ( lat_ind == (lat_size - 1) )
   --lat_ind;

 int lon_ind = norm_lon_ind ( (((double)lon - mo) / grid_step_lon) );
 //if ( lon_ind == (lon_size - 1) )
 //  cout<<lon_ind*nmc_grid_step<<endl ;

 // calculating cell` borders
 double left = lon_ind * grid_step_lon + mo;
 double right = ( lon_ind + 1 ) * grid_step_lon + mo;
 double bottom = lat_ind * grid_step_lat + ma;
 double top = ( lat_ind + 1 ) * grid_step_lat + ma;

 // extracting corner vectors
 vec3d LB = NMCVec->get_vec( lat_ind, lon_ind );
 vec3d LT = NMCVec->get_vec( lat_ind + 1, lon_ind );
 vec3d RB = NMCVec->get_vec( lat_ind, lon_ind + 1 );
 vec3d RT = NMCVec->get_vec( lat_ind + 1, lon_ind + 1 );

 // interpolating with proportion
 vec3d top_v = proport( LT, RT, (lon - left)/(right - left) );
 vec3d bot_v = proport( LB, RB, (lon - left)/(right - left) );

 return proport( bot_v, top_v, (lat - bottom)/(top - bottom) );
}

//---------------------------------------------------------------------

const static double Alpha {-1.0/sqrt(3.0)};
const static double Beta  {8.0*sqrt(2.0)/(3.0*sqrt(385.0))};

const static double k1 {sqrt( 3.0/( 4.0 * M_PI ) )};
const static double k2 {( 3.0 / 16.0 ) * sqrt( 385.0/( 2.0 * M_PI ) )};

void Vecfield::field1( const vec3d& x,
                       vec3d* pv )
{
  const double phi = atan2( x[1], x[0] );
  const double F =  4.0 * k2 * sin( 4 * phi );
  const double G =        k2 * cos( 4 * phi );
  const double Z = ( x[0]*x[0] + x[1]*x[1] );

  const vec3d dpsi = vec3d( Beta * F * x[1] * x[2] * Z,
                          - Beta * F * x[0] * x[2] * Z,
                            Alpha * k1 
                            + Beta*G * Z * ( 1 - 5 * x[2]*x[2] ) );

  *pv = glm::cross( x, dpsi );
}

////----------------------------------------------------------------------------
////--------------------------- NMC Vec Field ----------------------------------
////----------------------------------------------------------------------------
//
//vec3d* NMCVecfield::get_vec( const size_t& lat_i, const size_t& lon_i )
//{
//	try
//	{
//		return &grid.at( lat_i ).at( lon_i );
//	}
//	catch(const std::out_of_range& oor)
//	{
//		return nullptr;
//	}
//}
//vec3d* NMCVecfield::get_vec( const double& lat, const double& lon )
//{
//	try
//	{
//		return &grid.at( lat_indexer.at( lat ) ).at( lon_indexer.at( lon ) );
//	}
//	catch(const std::out_of_range& oor)
//	{
//		return nullptr;
//	}
//}
//NMCVecfield::GridNode NMCVecfield::get_node( const size_t& lat_i, const size_t& lon_i )
//{
//	try
//	{
//		return NMCVecfield::GridNode( lat_valuator.at( lat_i ), lon_valuator.at( lon_i ), grid.at( lat_i ).at( lon_i ) );
//	}
//	catch(const std::out_of_range& oor)
//	{
//		throw(oor);
//		//return nullptr;
//	}
//}
//NMCVecfield::GridNode NMCVecfield::get_node( const double& lat, const double& lon )
//{
//	try
//	{
//		return NMCVecfield::GridNode( lat, lon, grid.at( lat_indexer.at( lat ) ).at( lon_indexer.at( lon ) ) );
//	}
//	catch(const std::out_of_range& oor)
//	{
//		throw(oor);
//		//return nullptr;
//	}
//}
//
//void NMCVecfield::set_vec( const vec3d& value, const size_t& lat_i, const size_t& lon_i )
//{
//	try
//	{
//		grid.at( lat_i ).at( lon_i ) = value;
//	}
//	catch(const std::out_of_range& oor)
//	{
//		//error: "illegal index"
//		throw(oor);
//	}
//}
//void NMCVecfield::set_vec( const vec3d& value, const double& lat, const double& lon )
//{
//	try
//	{
//		grid.at( lat_indexer.at( lat ) ).at( lon_indexer.at( lon ) ) = value;
//	}
//	catch(const std::out_of_range& oor)
//	{
//		cout<<"dkjfghdfhg";
//		//error: "bad argument: no such coordinates"
//		throw(oor);
//	}
//}
//void NMCVecfield::set_node( const GridNode& GN, const size_t& lat_i, const size_t& lon_i )
//{
//	try
//	{
//		grid.at( lat_i ).at( lon_i ) = GN.value;
//		lat_indexer[ GN.lat ] = lat_i;
//		lon_indexer[ GN.lon ] = lon_i;
//		lat_valuator[ lat_i ] = GN.lat;
//		lon_valuator[ lon_i ] = GN.lon;
//	}
//	catch(const std::out_of_range& oor)
//	{
//		//error: "illegal index"
//		throw(oor);
//	}
//}
//
//void NMCVecfield::init_grid( const size_t& lat_s, const size_t& lon_s )
//{
//	clear();
//	grid.resize( lat_s, std::vector< vec3d > ( lon_s ) );
//}
//void NMCVecfield::clear()
//{
//	for( size_t i=0; i < grid.size(); ++i)
//		grid[i].clear();
//	grid.clear();
//	lat_indexer.clear();
//	lon_indexer.clear();
//	lat_valuator.clear();
//	lon_valuator.clear();
//}
