/*!

  \file mproperties.cc

  \brief Implementation of function mproperties( Globals& )

*/

#include "mproperties.hh"

void mproperties( Globals& siku )
{
  for ( auto & e: siku.es )
    {
      Material *pmat = &siku.ms[ e.imat ]; // short link to material

      // mass update
      double m = 0;
      for ( size_t i = 0; i < pmat->thickness_intervals.size(); ++i )
        {
          m += pmat->thickness_intervals[i] * pmat->rho[i] * e.gh[i];
        }

      e.m = e.A * m;
      e.I = e.m * e.i;          // moment of inertia update

      // current global position update
      e.Glob = Coordinates::loc_to_glob ( e.q, Coordinates::NORTH );
    }
}
