/*!

 \file globals.cc

 \brief Initialization and some processing for Globals structure

 */

#include "globals.hh"
#include "sikupy.hh"

void Globals::post_init()
{
  wind.init( wind.FIELD_SOURCE_TYPE );
//  if( wind.FIELD_SOURCE_TYPE == Vecfield::NMC )
//    Sikupy::read_nmc_vecfield ( *siku.wind.NMCVec, "wind" );


  for( auto& e : es )
    {
      // setting default (loaded from .py) velocity and rotation
      e.W = vec3d( -e.V.y / planet.R, e.V.x / planet.R, e.V.z );
    }
}
