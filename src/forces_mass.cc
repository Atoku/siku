/*!

  \file forces_mass.cc

  \brief Implementation of forces_mass function

*/

#include "forces_mass.hh"

void forces_mass( Globals& siku )
{
  for ( auto & e: siku.es )
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
      //e.F = 0.0016 * sqrt ( V.x * V.x + V.y * V.y + V.z * V.z ) * V * e.A;
      e.F = 0.0016 * V.length() * V * e.A;

      //std::cout<<"\t"<<e.F.x<<"\t"<<e.F.y<<"\t"<<e.F.z<<endl;

      //-------- WATER (STEADY) ----------
      // calculating element`s speed in local coords
      V = vec3d( e.W.y * siku.planet.R, -e.W.x * siku.planet.R, 0. );

      // interpolating currents speed
      vec3d W = siku.flows.get_at_lat_lon_rad ( Coordinates::norm_lat ( lat ),
                                         Coordinates::norm_lon ( lon ) );
      // transforming currents into local coords
      W = Coordinates::glob_to_loc( e.q, W );

      // velocity defference between ice element and water
      W -= V;

      // applying water forces
      e.F += 0.0045 * W.length() * W * e.A;

      //std::cout<<"\t"<<e.F.x<<"\t"<<e.F.y<<"\t"<<e.F.z<<endl;
      // element trace
      std::cout<<lat<<"\t"<<lon<<endl<<endl;

      ////////////////////////

    }
  
}
