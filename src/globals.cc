/*!

 \file globals.cc

 \brief Initialization and some processing for Globals structure

 */

#include "globals.hh"
#include "sikupy.hh"
#include "coordinates.hh"

using namespace Coordinates;

void Globals::post_init()
{
  wind.init( wind.FIELD_SOURCE_TYPE );
//  if( wind.FIELD_SOURCE_TYPE == Vecfield::NMC )
//    Sikupy::read_nmc_vecfield ( *siku.wind.NMCVec, "wind" );

  for( auto& e : es )
    {
      // setting default (loaded from .py) velocity and rotation
      double lat, lon;
      sph_by_quat( e.q, &lat, &lon );

      vec3d temp = glob_to_loc(
          e.q, geo_to_cart_surf_velo( lat, lon, e.V.x, e.V.y ) );

      // velocity must be inputed in East-North terms
      e.W = vec3d( -temp.y * planet.R_rec, temp.x * planet.R_rec, e.V.z );
    }
}
