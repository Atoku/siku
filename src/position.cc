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
      p = quat ( 1 - S, 0.5 * dt * e.W )  / ( 1 + S );
      e.q = glm::cross ( e.q, p );
      e.q = glm::normalize( e.q );
    }
}
