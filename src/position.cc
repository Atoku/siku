/*!

 \file position.cc

 \brief Position update using quaternions

 */

#include "position.hh"

void
position ( Globals& siku, const double dt )
{
  const double C = 1.0 / 16.0;
  quat p;

  for ( auto & e : siku.es )
    {
      double S = glm::dot ( e.W, e.W ) * dt * C;
      p = quat ( 1 - S, 0.5 * dt * e.W );
      e.q = glm::cross ( e.q, p ) / ( 1 + S );
    }
}
