/*!

  \file dynamics.cc

  \brief Implementation of dynamics function. The dynamics is update
  on velocities and angular velocity of each ice element depending on
  the forces and torques applied (and previously calculated). All
  velocities are coded in a single 3D angular velocity vector set in
  body frame.

*/

#include "dynamics.hh"

void dynamics( Globals& siku, const double dt )
{
  for ( auto & e: siku.es )
    {
      // first we create a vector of Super-Torque
      vec3d sT ( - e.F[1] / ( siku.planet.R * e.m ),
                   e.F[0] / ( siku.planet.R * e.m ),
                   e.N    / e.I );

      // and increment the angular velocity using it
      e.W += sT * dt;
    }
}
