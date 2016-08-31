/*!

  \file mproperties.cc

  \brief Implementation of function mproperties( Globals& )

*/

#include <assert.h>

#include "mproperties.hh"
#include "errors.hh"
#include "coordinates.hh"
#include "siku.hh"

using namespace Geometry;

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

  // minimal and maximal stress tensor components
  siku.SxxMax = siku.SxxMin = siku.es[0].Sxx;
  siku.SyyMax = siku.SyyMin = siku.es[0].Syy;
  siku.SxyMax = siku.SxyMin = siku.es[0].Sxy;
  siku.SyxMax = siku.SyxMin = siku.es[0].Syx;

  for ( Element & e: siku.es )
    {
//      if( e.flag & Element::F_ERRORED )
//        continue;
/////////////////
      // input test: element`s NaN checks
      bool nan_flag = false;

      if( (nan_flag |= NaN( e.q )) )
          cout<<"\nERROR: NaN q at element "<<e.id<<" !\n"
          <<e.q.w<<" "<<e.q.x<<" "<<e.q.y<<" "<<e.q.z<<" "<<"\n";

      nan_flag |= NaN( e.Glob );
      if( NaN( e.Glob ) )
          cout<<"\nERROR: NaN Glob at element "<<e.id<<" !\n"
          <<e.Glob.x<<" "<<e.Glob.y<<" "<<e.Glob.z<<" "<<"\n";

      nan_flag |= NaN( e.V );
      if( NaN( e.V ) )
          cout<<"\nERROR: NaN V at element "<<e.id<<" !\n"
          <<e.V.x<<" "<<e.V.y<<" "<<e.V.z<<" "<<"\n";

      nan_flag |= NaN( e.W );
      if( NaN( e.W ) )
          cout<<"\nERROR: NaN W at element "<<e.id<<" !\n"
          <<e.W.x<<" "<<e.W.y<<" "<<e.W.z<<" "<<"\n";

      if( nan_flag )
        {
          cout<<"NaN element "<<e.id<<endl;
          e.flag |= Element::F_ERRORED;
          fatal( 1, "NaN value in element");
        }
      assert( nan_flag ); // somehow - does not work!

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
      double m = 0.;
      for ( size_t i = 0; i < MAT_LAY_AMO; ++i )
        {
          m += pmat->layers[i].thickness * pmat->layers[i].rho * e.gh[i];
        }

      e.m = e.A * m   * siku.planet.R2; // mass update
      e.I = e.m * e.i * siku.planet.R2; // moment of inertia update

      // thickest layer
      e.h_main = e.gh[0];
      for( size_t i = 1; i < MAT_LAY_AMO; ++i )
        if( e.gh[i] > e.h_main )
          e.h_main = e.gh[i];

      // extremal stress tensor components
      if( e.Sxx > siku.SxxMax )  siku.SxxMax = e.Sxx;
      if( e.Sxx < siku.SxxMin )  siku.SxxMin = e.Sxx;
      if( e.Syy > siku.SyyMax )  siku.SyyMax = e.Syy;
      if( e.Syy < siku.SyyMin )  siku.SyyMin = e.Syy;
      if( e.Sxy > siku.SxyMax )  siku.SxyMax = e.Sxy;
      if( e.Sxy < siku.SxyMin )  siku.SxyMin = e.Sxy;
      if( e.Syx > siku.SyxMax )  siku.SyxMax = e.Syx;
      if( e.Syx < siku.SyxMin )  siku.SyxMin = e.Syx;

      // current global position update
      e.Glob = Coordinates::loc_to_glob ( e.q, Coordinates::NORTH );

      // marking element as it was already processed (required for save-load
      // runs where new elements may be added)
      e.flag |= Element::F_PROCESSED;

      // checking land-fastency condition
      if( (e.flag & Element::F_FREE) && (~e.flag & Element::F_FASTENED) &&
          (e.OA) && (e.OA > e.Amin*siku.phys_consts["fastency"]) )
        {
          e.flag &= ~( Element::F_FREE );//| Element::F_STEADY );
          e.flag |= Element::F_STATIC | Element::F_FASTENED;
// TODO: clean
//          cout<<"FAST!  "<<e.id<<": "<<e.OA<<", "<<e.Amin<<", "<<e.A<<endl;

        }
    }
}

// --------------------------------------------------------------------------

void clean_props( Globals& siku )
{
  for ( Element & e: siku.es )
    {
      // clearing all accumulated values
      e.F = nullvec3d;
      e.N = 0;
      e.OA = 0.;
      e.Amin = e.A;
      e.Sxx = 0.;
      e.Syy = 0.;
      e.Sxy = 0.;
      e.Syx = 0.;
    }
}
