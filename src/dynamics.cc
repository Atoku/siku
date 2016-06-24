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

//#include "planet.hh"

///////////
#include <iostream>

void
dynamics ( Globals& siku, const double dt )
{
  for ( auto & e : siku.es )
    {
//      if( e.flag & Element::F_ERRORED ) continue; // TODO: change or remove dis

      if ( e.flag & Element::F_STATIC ) //continue;
        {
          e.W = {};
          e.V = {};
          continue;
        }

      // first we create a vector of Super-Torque
//      vec3d sT ( -e.F[1] / ( siku.planet.R * e.m ),
//                 e.F[0] / ( siku.planet.R * e.m ), e.N / e.I );
      //// manual drag added
      // TODO: fix this factor - rotation slowdonw caused by water viscosity
      double c = 0.0 * siku.planet.R_rec * siku.time.get_dt(); //time scaling coz pseudoforce
      vec3d sT ( -e.F[1] / ( siku.planet.R * e.m ),
                 e.F[0] / ( siku.planet.R * e.m ), e.N / e.I - c * e.W.z );

      VERIFY( sT, string("dyn ") + to_string(e.m) + string(" | ") + to_string(e.F[0]) + "  " + to_string(e.F[1]) );

     // sT = vec3d ( -e.F[1] , e.F[0] , e.N / e.I - c * e.W.z );
      //vec3d sT = nullvec;

      // and increment the angular velocity using it (if not steady)
      if( ! ( e.flag & Element::F_STEADY ) )
        e.W += sT * dt;

      VERIFY( e.W, "dyn: W");

      // calculating local speed
      //e.V = vec3d( e.W.y , -e.W.x , 0. );
      e.V = vec3d( e.W.y * siku.planet.R , -e.W.x * siku.planet.R, 0. );
      VERIFY(e.V, "V in dyn");

    }
}
