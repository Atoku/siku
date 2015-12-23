/*!

  \file vecfield.cc

  \brief Implementation of vector field class.

*/

#include "vecfield.hh"
#include "errors.hh"
#include "sikupy.hh"

using namespace Coordinates;

//---------------------------------------------------------------------
//---------------- UTIL FUNCTIONS FOR INTERPOLATION -------------------
//---------------------------------------------------------------------

inline vec3d proport( const vec3d& d1, const vec3d& d2, const double& t )
{
//  return vec3d( d1.x + (d2.x - d1.x)*t,
//                d1.y + (d2.y - d1.y)*t,
//                d1.z + (d2.z - d1.z)*t );
  return d1 + ( d2 - d1 )*t;
}

//---------------------------------------------------------------------
//---------------------------------------------------------------------
//---------------------------------------------------------------------

Vecfield::Vecfield(const Source_Type& SOURCE_TYPE) :
FIELD_SOURCE_TYPE( SOURCE_TYPE )
{
  mode = MODE_VEC_STD_FIELD1;
  if( FIELD_SOURCE_TYPE == NMC )  NMCVec = new NMCVecfield;
 // cout<<NMCVec<<" Vecfield!!!!! "<<SOURCE_TYPE<<endl;
}

//---------------------------------------------------------------------

Vecfield::Vecfield()
{
  //NMCVec = new NMCVecfield;
  mode = MODE_VEC_STD_FIELD1;
}

//---------------------------------------------------------------------

void Vecfield::init ( const Source_Type& SOURCE_TYPE )
{
  mode = MODE_VEC_STD_FIELD1;
  FIELD_SOURCE_TYPE = SOURCE_TYPE;

  if( FIELD_SOURCE_TYPE == NMC )  NMCVec = new NMCVecfield;
}

//---------------------------------------------------------------------

Vecfield::~Vecfield()
{
  if( NMCVec ) delete NMCVec; // safe deletion
}

//---------------------------------------------------------------------

void Vecfield::get_at_xy( const vec3d& x, vec3d* pv )
{
  switch ( mode )
    {
      case MODE_VEC_STD_FIELD1: 
        field1( x, pv );
        break;

      default:
        fatal( 1, "Not standard wind field is not implemented yet" );
        break;
    }
}

//---------------------------------------------------------------------

vec3d Vecfield::get_at_lat_lon_rad( double lat,  double lon )
{
  //draft: the simplest interpolation

  // inner testing
  if( FIELD_SOURCE_TYPE == TEST )
    {
      return Coordinates::geo_to_cart_surf_velo ( lat, lon, -10., 1. );
      //return Coordinates::geo_to_cart_surf_velo( lat lon, 10, 0 );
    }

  // default return
  if( ! NMCVec )  return vec3d(0., 0., 0.);

  // input params check
  if( lat != lat || lon != lon )
    {
      cout<<"\nERROR: NaN lat or/and lon!\n"<<lat<<" "<<lon<<"\n\n";
      fatal( 1, "interpolation args are NaN!");
    }

  // normalizing latitude in range [0, Pi],
  // longitude in range [0, 2Pi] (for proper indexing)
  lat = norm_lat ( lat ) + M_PI/2.;
  lon = norm_lon ( lon );

  // refreshing grid sizes
  lon_size = NMCVec->get_lon_size ();
  lat_size = NMCVec->get_lat_size ();

  /*
   * TODO: CHECK DIS DAM INDEXES!
   */
  // calculating current cell indexes
  int lat_ind = norm_lat_ind( ((double)lat / nmc_grid_step) );
  if ( lat_ind == (lat_size - 1) )
    --lat_ind;

  int lon_ind = norm_lon_ind ( ((double)lon / nmc_grid_step) );
  //if ( lon_ind == (lon_size - 1) )
  //  cout<<lon_ind*nmc_grid_step<<endl ;

  // calculating cell` borders
  double left = lon_ind * nmc_grid_step;
  double right = ( lon_ind + 1 ) * nmc_grid_step;
  double bottom = lat_ind * nmc_grid_step;
  double top = ( lat_ind + 1 ) * nmc_grid_step;

  // extracting corner vectors
  vec3d LB = NMCVec->get_vec( lat_ind, lon_ind );
  vec3d LT = NMCVec->get_vec( lat_ind + 1, lon_ind );
  vec3d RB = NMCVec->get_vec( lat_ind, lon_ind + 1 );
  vec3d RT = NMCVec->get_vec( lat_ind + 1, lon_ind + 1 );

  // interpolating with proportion
  vec3d top_v = proport( LT, RT, (lon - left)/(right - left) );
  vec3d bot_v = proport( LB, RB, (lon - left)/(right - left) );

//// TEST OUTPUT
//  std::cout<<LB.x<<"\t"<<LB.y<<"\t"<<LB.z<<"\n";
//  std::cout<<LT.x<<"\t"<<LT.y<<"\t"<<LT.z<<"\n";
//  std::cout<<RB.x<<"\t"<<RB.y<<"\t"<<RB.z<<"\n";
//  std::cout<<RT.x<<"\t"<<RT.y<<"\t"<<RT.z<<"\n";
//
//  std::cout<<top_v.x<<"\t"<<top_v.y<<"\t"<<top_v.z<<"\n";
//  std::cout<<bot_v.x<<"\t"<<bot_v.y<<"\t"<<bot_v.z<<"\n";
//
//  vec3d V = proport( bot_v, top_v, (lat - bottom)/(top - bottom) );
//
//  std::cout<<V.x<<"\t"<<V.y<<"\t"<<V.z<<"\n";
//
//  if( (lon - left)/(right - left) > 1. ||
//      (lat - bottom)/(top - bottom) > 1. ||
//      (lon - left)/(right - left) < 0. ||
//      (lat - bottom)/(top - bottom) < 0.
//      )
//    {
//      cout<<"WOUWOU\n";
//      cout<<rad_to_deg(left)<<"\t"<<rad_to_deg(right)<<"\t"<<
//            rad_to_deg(bottom)<<"\t"<<rad_to_deg(top)<<"\n";
//      cout<<rad_to_deg(lon)<<"\t"<<rad_to_deg(lat)<<"\n";
//      cin.get();
//    }

  return proport( bot_v, top_v, (lat - bottom)/(top - bottom) );
}

//---------------------------------------------------------------------

const static double Alpha {-1.0/sqrt(3.0)};
const static double Beta  {8.0*sqrt(2.0)/(3.0*sqrt(385.0))};

const static double k1 {sqrt( 3.0/( 4.0 * M_PI ) )};
const static double k2 {( 3.0 / 16.0 ) * sqrt( 385.0/( 2.0 * M_PI ) )};

void Vecfield::field1( const vec3d& x,
                       vec3d* pv )
{
  const double phi = atan2( x[1], x[0] );
  const double F =  4.0 * k2 * sin( 4 * phi );
  const double G =        k2 * cos( 4 * phi );
  const double Z = ( x[0]*x[0] + x[1]*x[1] );

  const vec3d dpsi = vec3d( Beta * F * x[1] * x[2] * Z,
                          - Beta * F * x[0] * x[2] * Z,
                            Alpha * k1 
                            + Beta*G * Z * ( 1 - 5 * x[2]*x[2] ) );

  *pv = glm::cross( x, dpsi );
}

////----------------------------------------------------------------------------
////--------------------------- NMC Vec Field ----------------------------------
////----------------------------------------------------------------------------
//
//vec3d* NMCVecfield::get_vec( const size_t& lat_i, const size_t& lon_i )
//{
//	try
//	{
//		return &grid.at( lat_i ).at( lon_i );
//	}
//	catch(const std::out_of_range& oor)
//	{
//		return nullptr;
//	}
//}
//vec3d* NMCVecfield::get_vec( const double& lat, const double& lon )
//{
//	try
//	{
//		return &grid.at( lat_indexer.at( lat ) ).at( lon_indexer.at( lon ) );
//	}
//	catch(const std::out_of_range& oor)
//	{
//		return nullptr;
//	}
//}
//NMCVecfield::GridNode NMCVecfield::get_node( const size_t& lat_i, const size_t& lon_i )
//{
//	try
//	{
//		return NMCVecfield::GridNode( lat_valuator.at( lat_i ), lon_valuator.at( lon_i ), grid.at( lat_i ).at( lon_i ) );
//	}
//	catch(const std::out_of_range& oor)
//	{
//		throw(oor);
//		//return nullptr;
//	}
//}
//NMCVecfield::GridNode NMCVecfield::get_node( const double& lat, const double& lon )
//{
//	try
//	{
//		return NMCVecfield::GridNode( lat, lon, grid.at( lat_indexer.at( lat ) ).at( lon_indexer.at( lon ) ) );
//	}
//	catch(const std::out_of_range& oor)
//	{
//		throw(oor);
//		//return nullptr;
//	}
//}
//
//void NMCVecfield::set_vec( const vec3d& value, const size_t& lat_i, const size_t& lon_i )
//{
//	try
//	{
//		grid.at( lat_i ).at( lon_i ) = value;
//	}
//	catch(const std::out_of_range& oor)
//	{
//		//error: "illegal index"
//		throw(oor);
//	}
//}
//void NMCVecfield::set_vec( const vec3d& value, const double& lat, const double& lon )
//{
//	try
//	{
//		grid.at( lat_indexer.at( lat ) ).at( lon_indexer.at( lon ) ) = value;
//	}
//	catch(const std::out_of_range& oor)
//	{
//		cout<<"dkjfghdfhg";
//		//error: "bad argument: no such coordinates"
//		throw(oor);
//	}
//}
//void NMCVecfield::set_node( const GridNode& GN, const size_t& lat_i, const size_t& lon_i )
//{
//	try
//	{
//		grid.at( lat_i ).at( lon_i ) = GN.value;
//		lat_indexer[ GN.lat ] = lat_i;
//		lon_indexer[ GN.lon ] = lon_i;
//		lat_valuator[ lat_i ] = GN.lat;
//		lon_valuator[ lon_i ] = GN.lon;
//	}
//	catch(const std::out_of_range& oor)
//	{
//		//error: "illegal index"
//		throw(oor);
//	}
//}
//
//void NMCVecfield::init_grid( const size_t& lat_s, const size_t& lon_s )
//{
//	clear();
//	grid.resize( lat_s, std::vector< vec3d > ( lon_s ) );
//}
//void NMCVecfield::clear()
//{
//	for( size_t i=0; i < grid.size(); ++i)
//		grid[i].clear();
//	grid.clear();
//	lat_indexer.clear();
//	lon_indexer.clear();
//	lat_valuator.clear();
//	lon_valuator.clear();
//}
