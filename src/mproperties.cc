/*!

  \file mproperties.cc

  \brief Implementation of function mproperties( Globals& )

*/

#include "mproperties.hh"



void mproperties( Globals& siku )
{
  size_t size = siku.es.size();
  int count = 0;
  Element el;
  for( size_t i = 0; i < size; ++i )
    {
      if( siku.es[i].ERRORED )
        {
          //std::swap( siku.es[i], siku.es[--size] );
          //el = siku.es[i];
          siku.es[i] = siku.es[--size];
          //siku.es[size] = el;
          siku.es.pop_back();
          count++;
        }
    }
  if(count)
    cout<<"Cleared: "<<count<<" Elements after cleaning errors: "
        <<siku.es.size()<<endl;

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

      /*
       * TODO: clear this mess with planet.R, planet.R2 all around the code
       */
      ///////////// AAAAH!! Area and i has been calculated for UNIT SPHERE!
      ///////////// So they are scaled manually down here
      ///////////// And this should be removed (fixed, moved somewhere else...)
      e.m *= siku.planet.R2;
      e.I *= siku.planet.R2;

//cout<<e.m<<"\t"<<e.I<<"\t"<<e.A*siku.planet.R2<<endl;
//cin.get();
      // current global position update
      e.Glob = Coordinates::loc_to_glob ( e.q, Coordinates::NORTH );

      // clearing the force and the torque
      e.F = nullvec;
      e.N = 0;
    }
}
