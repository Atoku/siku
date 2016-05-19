/*!

  \file forces_mass.cc

  \brief Implementation of forces_mass function

*/

#include "forces_mass.hh"
#include "errors.hh"
#include "coordinates.hh"

using namespace Geometry;
using namespace Coordinates;

//////testing
#include <iostream>


// ----------------------------- local utils --------------------------------

inline void _drag_factors( Globals& siku, Element& e,
                           double& water_factor, double& wind_factor )
{
  // yet simple scaling by constants from python. May me changed to
  // multiparametric algorithm later.
  water_factor = e.anchority * siku.phys_consts["anchority"];
  wind_factor  = e.windage   * siku.phys_consts["windage"];
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void forces_mass( Globals& siku )
{
  for ( size_t i = 0; i < siku.es.size (); ++i )
    {
      if( siku.es[i].flag & Element::F_ERRORED )
        continue;

      // i just don`t like range-based loops
      auto & e = siku.es[i];

      // force scaling
      double wnd_fact, wat_fact;
      _drag_factors( siku, e, wat_fact, wnd_fact );

      //-------- WIND ----------

      // acquiring element` position in terms lat-lon
      double lat, lon;
      Coordinates::sph_by_quat ( e.q, &lat, &lon );
      
      // interpolating wind speed near element`s mass center
      vec3d V = siku.wind.get_at_lat_lon_rad ( Coordinates::norm_lat ( lat ),
                                               Coordinates::norm_lon ( lon ) );

      // transforming to local coordinates
      V = Coordinates::glob_to_loc( e.q, V );

      // calculating local Force (draft)
      e.F += V * abs( V ) * e.A * siku.planet.R2 * wnd_fact;
      VERIFY( e.F , "wind forces mass" );

      //-------- WATER (yet steady) ----------

      // calculating element`s speed in local coords
      V = e.V;

      // interpolating currents speed
      // !!check for earth.R scaling
      vec3d W = siku.flows.get_at_lat_lon_rad ( Coordinates::norm_lat ( lat ),
                                         Coordinates::norm_lon ( lon ) );
      // transforming currents into local coords
      W = Coordinates::glob_to_loc( e.q, W );

      // velocity defference between ice element and water
      W -= V;

      // applying water forces
      e.F += W * abs( W ) * e.A * siku.planet.R2 * wat_fact;
      VERIFY( e.F, "water in forces mass");
    }
  
  // manual forces
  for( size_t i = 0; i < siku.man_inds.size(); ++i )
    {
      // indexes of manually added forces
      size_t I = siku.man_inds[i];
      vec3d tv;

      double lat, lon;
      sph_by_quat ( siku.es[I].q, &lat, &lon );

      tv = glob_to_loc ( siku.es[I].q, geo_to_cart_surf_velo(
          lat, lon, siku.man_forces[i].x, siku.man_forces[i].y ) );

      vec3d F = tv;
      double trq = siku.man_forces[i].z;

      // TODO: check planet.R scaling
      siku.es[I].flag |= Element::F_SPECIAL;
      siku.es[I].F += F * siku.planet.R;
      siku.es[I].N += trq;
    }

}
