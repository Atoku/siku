/*!

 \file globals.cc

 \brief Initialization and some processing for Globals structure

 */

#include "globals.hh"

void Globals::post_init()
{
  for( auto& e : es )
    {
      // setting default (loaded from .py) velocity and rotation
      e.W = vec3d( -e.V.y / planet.R, e.V.x / planet.R, e.V.z );
    }
}
