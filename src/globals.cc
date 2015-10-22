/*!

 \file globals.cc

 \brief Initialization and some processing for Globals structure

 */

#include "globals.hh"
#include "sikupy.hh"
#include "coordinates.hh"

#include "fstream"
#include <vector>

using namespace Coordinates;

void Globals::post_init()
{
  wind.init( wind.FIELD_SOURCE_TYPE );
//  if( wind.FIELD_SOURCE_TYPE == Vecfield::NMC )
//    Sikupy::read_nmc_vecfield ( *siku.wind.NMCVec, "wind" );

  for( auto& e : es )
    {
      // setting default (loaded from .py) velocity and rotation
      double lat, lon;
      sph_by_quat( e.q, &lat, &lon );

      vec3d temp = glob_to_loc(
          e.q, geo_to_cart_surf_velo( lat, lon, e.V.x, e.V.y ) );

      // velocity must be inputed in East-North terms
      e.W = vec3d( -temp.y * planet.R_rec, temp.x * planet.R_rec, e.V.z );
    }

  if( mark_boarders )
    {
      cout<<"preparing boarders\n";

      // reading boarders
      std::ifstream in( board_file.c_str() );
      std::vector<vec3d> points; // global (x,y,z)
      double d1, d2;
      vec3d point;

      while( !in.eof() )
      {
        in >> d1 >> d2;

        point =
            sph_to_cart( 1., M_PI / 2. - deg_to_rad( d2 ), deg_to_rad( d1 ) );

        points.push_back( point );
      }

      in.close();

      int II=0;
      // marking boarders by boarder points (2d approximation)
      size_t size = points.size();
      for( auto& e : es )
        {
          for( size_t i = 0; i < size; ++i )
            {
              if( e.contains( points[i] ) )
                {
                  std::swap( points[i], points[--size] );
                  points.pop_back();

                  e.flag = ( e.flag &~ Element::F_MOVE_FLAG ) //clear move flag
                      | Element::F_STATIC; //mark as static
                }
            }
        }
      cout<<"done\n\n";
    }
}
