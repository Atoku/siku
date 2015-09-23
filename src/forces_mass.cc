/*!

  \file forces_mass.cc

  \brief Implementation of forces_mass function

*/

#include "forces_mass.hh"
#include "errors.hh"

//////testing
#include <iostream>


void forces_mass( Globals& siku )
{
  for ( size_t i = 0; i < siku.es.size (); ++i )
    {
      // i just don`t like range-based loops
      auto & e = siku.es[i];

      // input test: element`s quat NaN check
      if( e.q.w != e.q.w || e.q.x != e.q.x ||
          e.q.y != e.q.y || e.q.z != e.q.z )
        {
          cout<<"\nERROR: NaN quat at element \n"<<i<<" !\n";
          cout<<e.q.w<<" "<<e.q.x<<" "<<e.q.y<<" "<<e.q.z<<" "<<"\n";
          fatal( 1, "NaN value in quat");
        }

      //-------- WIND----------
      // acquiring element` position in terms lat-lon
      double lat, lon;
      Coordinates::sph_by_quat ( e.q, &lat, &lon );
      
      // interpolating wind speed near element`s mass center
      vec3d V = siku.wind.get_at_lat_lon_rad ( Coordinates::norm_lat ( lat ),
                                               Coordinates::norm_lon ( lon ) );

//      cout<<V.x<<"\t"<<V.y<<"\t"<<V.z<<"\n";

      // transforming to local coordinates
      V = Coordinates::glob_to_loc( e.q, V );

 //     cout<<V.x<<"\t"<<V.y<<"\t"<<V.z<<"\n";

      // calculating local Force (draft)
      // V.lenght() always equals to 3 - it`s amount of components!!!
      e.F = 0.0016 * vec_len( V ) * V * e.A;
      //e.F = 0.0016 * sqrt( V.x*V.x + V.y*V.y + V.z*V.z ) * V * e.A;

      //-------- WATER (STEADY) ----------
      // calculating element`s speed in local coords
      V = vec3d( e.W.y * siku.planet.R, -e.W.x * siku.planet.R, 0. );

      // interpolating currents speed
      vec3d W = siku.flows.get_at_lat_lon_rad ( Coordinates::norm_lat ( lat ),
                                         Coordinates::norm_lon ( lon ) );
      // transforming currents into local coords
      W = Coordinates::glob_to_loc( e.q, W );

      // velocity defference between ice element and water
      W -= V;

 //     cout<<W.x<<"\t"<<W.y<<"\t"<<W.z<<"\n\n";
      // applying water forces
      e.F += 0.0045 * vec_len( W ) * W * e.A;

      // element trace
//      std::cout<<"trace: "<<Coordinates::rad_to_deg((lat))
//      <<"\t"<<Coordinates::rad_to_deg((lon))<<endl;


    }
  
}
