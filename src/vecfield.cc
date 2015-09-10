/*!

  \file vecfield.cc

  \brief Implementation of vector field class.

*/

#include "vecfield.hh"
#include "errors.hh"

//---------------------------------------------------------------------

Vecfield::Vecfield()
{
  mode = MODE_VEC_STD_FIELD1;
}

//---------------------------------------------------------------------

void Vecfield::get_at_xy( const vec3d& x,
                          vec3d* pv )
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

//----------------------------------------------------------------------------
//--------------------------- NMC Vec Field ----------------------------------
//----------------------------------------------------------------------------

vec3d* NMCVecfield::get_vec( const size_t& lat_i, const size_t& lon_i )
{
	try
	{
		return &grid.at( lat_i ).at( lon_i );
	}
	catch(const std::out_of_range& oor)
	{
		return nullptr;
	}
}
vec3d* NMCVecfield::get_vec( const double& lat, const double& lon )
{
	try
	{
		return &grid.at( lat_indexer.at( lat ) ).at( lon_indexer.at( lon ) );
	}
	catch(const std::out_of_range& oor)
	{
		return nullptr;
	}
}
NMCVecfield::GridNode NMCVecfield::get_node( const size_t& lat_i, const size_t& lon_i )
{
	try
	{
		return NMCVecfield::GridNode( lat_valuator.at( lat_i ), lon_valuator.at( lon_i ), grid.at( lat_i ).at( lon_i ) );
	}
	catch(const std::out_of_range& oor)
	{
		throw(oor);
		//return nullptr;
	}
}
NMCVecfield::GridNode NMCVecfield::get_node( const double& lat, const double& lon )
{
	try
	{
		return NMCVecfield::GridNode( lat, lon, grid.at( lat_indexer.at( lat ) ).at( lon_indexer.at( lon ) ) );
	}
	catch(const std::out_of_range& oor)
	{
		throw(oor);
		//return nullptr;
	}
}

void NMCVecfield::set_vec( const vec3d& value, const size_t& lat_i, const size_t& lon_i )
{
	try
	{
		grid.at( lat_i ).at( lon_i ) = value;
	}
	catch(const std::out_of_range& oor)
	{
		//error: "illegal index"
		throw(oor);
	}
}
void NMCVecfield::set_vec( const vec3d& value, const double& lat, const double& lon )
{
	try
	{
		grid.at( lat_indexer.at( lat ) ).at( lon_indexer.at( lon ) ) = value;
	}
	catch(const std::out_of_range& oor)
	{
		cout<<"dkjfghdfhg";
		//error: "bad argument: no such coordinates"
		throw(oor);
	}
}
void NMCVecfield::set_node( const GridNode& GN, const size_t& lat_i, const size_t& lon_i )
{
	try
	{
		grid.at( lat_i ).at( lon_i ) = GN.value;
		lat_indexer[ GN.lat ] = lat_i;
		lon_indexer[ GN.lon ] = lon_i;
		lat_valuator[ lat_i ] = GN.lat;
		lon_valuator[ lon_i ] = GN.lon;
	}
	catch(const std::out_of_range& oor)
	{
		//error: "illegal index"
		throw(oor);
	}
}

void NMCVecfield::init_grid( const size_t& lat_s, const size_t& lon_s )
{
	clear();
	grid.resize( lat_s, std::vector< vec3d > ( lon_s ) );
}
void NMCVecfield::clear()
{
	for( size_t i=0; i < grid.size(); ++i)
		grid[i].clear();
	grid.clear();
	lat_indexer.clear();
	lon_indexer.clear();
	lat_valuator.clear();
	lon_valuator.clear();
}
