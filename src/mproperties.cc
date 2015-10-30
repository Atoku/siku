/*!

  \file mproperties.cc

  \brief Implementation of function mproperties( Globals& )

*/

#include "mproperties.hh"
#include "errors.hh"

#include "coordinates.hh"

void mproperties( Globals& siku )
{
//  size_t size = siku.es.size();
//  int count = 0;
//  Element el;
//  for( size_t i = 0; i < size; ++i )
//    {
//      if( siku.es[i].ERRORED )
//        {
//          //std::swap( siku.es[i], siku.es[--size] );
//          //el = siku.es[i];
//          siku.es[i] = siku.es[--size];
//          //siku.es[size] = el;
//          siku.es.pop_back();
//          count++;
//        }
//    }
//  if(count)
//    cout<<"Cleared: "<<count<<" Elements after cleaning errors: "
//        <<siku.es.size()<<endl;

  for ( auto & e: siku.es )
    {
      if( e.flag & Element::F_ERRORED )
        continue;
/////////////////
      // input test: element`s NaN checks
      bool nan_flag = false;

      if( (nan_flag |= NaN_q( e.q )) )
          cout<<"\nERROR: NaN q at element \n"<<e.id<<" !\n"
          <<e.q.w<<" "<<e.q.x<<" "<<e.q.y<<" "<<e.q.z<<" "<<"\n";

      nan_flag |= NaN_v( e.Glob );
      if( NaN_v( e.Glob ) )
          cout<<"\nERROR: NaN Glob at element \n"<<e.id<<" !\n"
          <<e.Glob.x<<" "<<e.Glob.y<<" "<<e.Glob.z<<" "<<"\n";

      nan_flag |= NaN_v( e.V );
      if( NaN_v( e.V ) )
          cout<<"\nERROR: NaN V at element \n"<<e.id<<" !\n"
          <<e.V.x<<" "<<e.V.y<<" "<<e.V.z<<" "<<"\n";

      nan_flag |= NaN_v( e.W );
      if( NaN_v( e.W ) )
          cout<<"\nERROR: NaN W at element \n"<<e.id<<" !\n"
          <<e.W.x<<" "<<e.W.y<<" "<<e.W.z<<" "<<"\n";

      if( nan_flag )
        {
          cout<<"NaN element "<<e.id<<endl;
          e.flag |= Element::F_ERRORED;
          //fatal( 1, "NaN value in element");
        }

//      if( e.flag & Element::F_SPECIAL )
//        {
//          double lat, lon;
//          Coordinates::sph_by_quat(e.q, &lat, &lon);
//          cout<<"special "<<e.id<<"\t"<<Coordinates::rad_to_deg(lon)<<"\t"
//              <<Coordinates::rad_to_deg(lat);
//        }
//////////////

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
