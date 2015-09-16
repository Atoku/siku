/*!

 \file dynamics.cc

 \brief Implementation of dynamics function. The dynamics is update
 on velocities and angular velocity of each ice element depending on
 the forces and torques applied (and previously calculated). All
 velocities are coded in a single 3D angular velocity vector set in
 body frame.

 */

#include "dynamics.hh"

///////////
#include "coordinates.hh"
#include <iostream>

void
dynamics ( Globals& siku, const double dt )
{
  for ( auto & e : siku.es )
    {
      //////// TESTING ///////

      double lat, lon;
      Coordinates::sph_by_quat ( e.q, &lat, &lon );
      //std::cout<<Coordinates::rad_to_deg(lat)<<"\t"<<Coordinates::rad_to_deg(lon)<<"\n";

      vec3d V = siku.wind.get_at_lat_lon_rad ( Coordinates::norm_lat ( lat ),
                                               Coordinates::norm_lon ( lon ) );
      //std::cout<<"Wind:  "<<V.x<<"\t"<<V.y<<"\t"<<V.z<<"\n";

      e.F = 0.0016 * sqrt ( V.x * V.x + V.y * V.y + V.z * V.z ) * V * e.A;

      //std::cout<<"Force:  "<<e.F.x<<"\t"<<e.F.y<<"\t"<<e.F.z<<"\n";
      ////////////////////////

      // first we create a vector of Super-Torque
      vec3d sT ( -e.F[1] / ( siku.planet.R * e.m ),
                 e.F[0] / ( siku.planet.R * e.m ), e.N / e.I );

      // and increment the angular velocity using it
      e.W += sT * dt;
    }
}
