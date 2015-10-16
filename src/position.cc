/*!

 \file position.cc

 \brief Position update using quaternions

 */

#include "position.hh"

#include <iostream>
using namespace std;

void
position ( Globals& siku, const double dt )
{
  static const double C = 1.0 / 16.0;
  quat p;

  for ( auto & e : siku.es )
    {
      if ( e.flag & Element::F_STATIC ) continue;

      double S = glm::dot ( e.W, e.W ) * dt * dt * C;
      //p = quat ( 1 - S, 0.5 * dt * e.W )  / ( 1 + S );
      //no self rotation
      p = quat ( 1 , 0.5 * dt * vec3d( e.W.x, e.W.y, 0 ) );
      //e.q = glm::cross ( e.q, p );

////////////////
      //translation
      e.q = glm::cross ( e.q, p );
      p = quat(1, 0.5*dt*vec3d(0,0,e.W.z));
      //rotation
      quat t = glm::cross ( e.q, p );
      e.W = Coordinates::loc_to_loc( t, e.q, e.W );
      e.q = t;
////////////////////

      e.q = glm::normalize( e.q );
    }
}
