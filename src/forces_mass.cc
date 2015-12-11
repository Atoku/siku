/*!

  \file forces_mass.cc

  \brief Implementation of forces_mass function

*/

#include "forces_mass.hh"
#include "errors.hh"

using namespace Geometry;

//////testing
#include <iostream>

double test_adjuster = 0.5;

void forces_mass( Globals& siku )
{

  for ( size_t i = 0; i < siku.es.size (); ++i )
    {
      if( siku.es[i].flag & Element::F_ERRORED )
        continue;

      // i just don`t like range-based loops
      auto & e = siku.es[i];

      //-------- WIND----------
      // acquiring element` position in terms lat-lon
      double lat, lon;
      Coordinates::sph_by_quat ( e.q, &lat, &lon );
      
      // interpolating wind speed near element`s mass center
      vec3d V = siku.wind.get_at_lat_lon_rad ( Coordinates::norm_lat ( lat ),
                                               Coordinates::norm_lon ( lon ) );

      // transforming to local coordinates
      V = Coordinates::glob_to_loc( e.q, V );

      // calculating local Force (draft)
      // V.lenght() always equals to 3 - it`s amount of components!!!
      e.F += 0.0016 * abs( V ) * V * e.A * siku.planet.R2;
      //e.F = 0.0016 * sqrt( V.x*V.x + V.y*V.y + V.z*V.z ) * V * e.A;

      //-------- WATER (STEADY) ----------
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
      e.F += 0.0045 * 1 * abs( W ) * W * e.A * siku.planet.R2 *test_adjuster;

      // experimental water torque
      //e.N += 0.0045 * e.W.z * e.W.z * e.I * test_adjuster;

    }
  
}
