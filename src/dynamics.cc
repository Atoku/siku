/*!

 \file dynamics.cc

 \brief Implementation of dynamics function. The dynamics is update
 on velocities and angular velocity of each ice element depending on
 the forces and torques applied (and previously calculated). All
 velocities are coded in a single 3D angular velocity vector set in
 body frame.

 */

#include "dynamics.hh"
#include "coordinates.hh"

///////////
#include <iostream>

void
dynamics ( Globals& siku, const double dt )
{
  for ( auto & e : siku.es )
    {
      //////// TESTING ///////
      //-------- WIND----------
      // acquiring element` position in terms lat-lon
      double lat, lon;
      Coordinates::sph_by_quat ( e.q, &lat, &lon );

      // interpolating wind speed near element`s mass centre
      vec3d V = siku.wind.get_at_lat_lon_rad ( Coordinates::norm_lat ( lat ),
                                               Coordinates::norm_lon ( lon ) );
      // transforming to local coordinates
      V = Coordinates::glob_to_loc( e.q, V );

      // calculating local Force (draft)
      e.F = 0.0016 * sqrt ( V.x * V.x + V.y * V.y + V.z * V.z ) * V * e.A;

      //-------- WATER (STEADY) ----------
      //!!!!!!!! ELEMENT SPEED SHOULD BE ADDED TO IT`S PROPS !!!!!!!!!!!!
      ////////////////////////

      // first we create a vector of Super-Torque
      vec3d sT ( -e.F[1] / ( siku.planet.R * e.m ),
                 e.F[0] / ( siku.planet.R * e.m ), e.N / e.I );

      // and increment the angular velocity using it
      e.W += sT * dt;
    }
}
