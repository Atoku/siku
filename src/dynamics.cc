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
#include "contact_force.hh"

///////////
#include <iostream>

void
dynamics ( Globals& siku, const double dt )
{
  for ( auto& c : siku.ConDet.cont )
    contact_push( siku.es[ c.i1 ], siku.es[ c.i2 ], siku );

  for ( auto & e : siku.es )
    {
      //cout<<"%%% "<<e.I<<endl;
      //cout<<"%%% "<<e.m<<endl;

      if ( e.flag & F_STATIC ) continue;

      // first we create a vector of Super-Torque
      vec3d sT ( -e.F[1] / ( siku.planet.R * e.m ),
                 e.F[0] / ( siku.planet.R * e.m ), e.N / e.I );

      // and increment the angular velocity using it
      e.W += sT * dt;

      // calculating local speed
      e.V = vec3d( e.W.y * siku.planet.R , -e.W.x * siku.planet.R, 0. );

//
//      for( auto p : e.P )
//        {
//          double lat, lon;
//          Coordinates:: Coordinates::loc_to_glob(e.q,p);
//          cout<<p.x<<"\t"<<p.y<<endl;
//        }
//      cout<<endl;
//cin.get();
      ///////// Spin test: different angle velocities for tester`s choice
      //e.W.z = 0.00001;
      ///// for comparison try also"
      //e.W.z = 0.001;
      //e.W.z = 0.0000001;

    }
}
