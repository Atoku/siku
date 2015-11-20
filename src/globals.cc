/*!

 \file globals.cc

 \brief Initialization and some processing for Globals structure

 */

#include "globals.hh"
#include "sikupy.hh"
#include "coordinates.hh"

#include "fstream"

using namespace Coordinates;

// --------------------------------------------------------------------------

Globals::Globals()
{
  // default empty-string first members
  cons.push_back( std::string( "" ) );
  mons.push_back( std::string( "" ) );
}

// --------------------------------------------------------------------------

void Globals::post_init()
{
  wind.init( wind.FIELD_SOURCE_TYPE );
//  if( wind.FIELD_SOURCE_TYPE == Vecfield::NMC )
//    Sikupy::read_nmc_vecfield ( *siku.wind.NMCVec, "wind" );

  for( size_t i =0; i < es.size(); ++i )
    {
      // setting new elements id
      es[i].id = i;

      if( es[i].flag & Element::F_PROCESSED ) continue; //only for new elements


      // setting default (loaded from .py) velocity and rotation
      double lat, lon;
      sph_by_quat( es[i].q, &lat, &lon );

      vec3d temp = glob_to_loc(
          es[i].q, geo_to_cart_surf_velo( lat, lon, es[i].V.x, es[i].V.y ) );

      // velocity must be inputed in East-North terms
      es[i].W = vec3d( -temp.y * planet.R_rec, temp.x * planet.R_rec,
                       es[i].V.z );
    }

  if( mark_boarders )
    {
      cout<<"Marking boarders with points\n";

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

      // marking boarders by boarder points (2d approximation)
      size_t size = points.size();
      for( auto& e : es )
        {
          if( e.flag & Element::F_PROCESSED )  // only for new elements
            continue;

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
      cout<<"Done\n\n";
    }
}

// --------------------------------------------------------------------------

void Globals::add_monit( std::string& mon, Element& e )
{
  // search for matching registered monitor name
  for( size_t i =0; i < mons.size(); ++i )
    {
      if( ! mons[i].compare( mon ) ) // 0 means equal
        {
          //set found monitor name index
          e.mon_ind = i;
          return;
        }
    }

  // if name was not found - add new one
  mons.push_back( mon );
  e.mon_ind = mons.size() - 1;
  return;
}

// --------------------------------------------------------------------------

void Globals::add_contr( std::string& con, Element& e )
{
  // YET UNUSED
  /*
   * TODO: Add control functions all around the program.
   */
}

