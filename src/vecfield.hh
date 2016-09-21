/*!

  \file vecfield.hh
  
  \brief Vector field class. 
  
  A basic class to contains air wind and sea streams vector fields,
  produce interpolation and provide values of the vector field in
  particular points.

 */

#ifndef VECFIELD_HH
#define VECFIELD_HH

//#include <map>
//#include <vector>
//#include <string>
//#include <stdexcept>

#include "siku.hh"
#include "nmc_reader.hh"
#include "coordinates.hh"

#include <cmath>

////////////////
#include <iostream>

/*! \brief Vector field to contain air wind and sea streams and
    produce interpolation of the vector field in a particular point on
    sphere.

 */
class Vecfield
{
public:

  //! \brief Defines the source type for the vector field used
  enum Source_Type : unsigned long { NONE, TEST, NMC };

  //! \brief Flag points to standard field from Fuselier paper
  static const int MODE_VEC_STD_FIELD1 { 1 };

  //-------------------------------------------------------------------------

  //! \brief Flag for vecfield source type
  Source_Type FIELD_SOURCE_TYPE { NONE };

  //! \brief NMC wind grid pointer. If the source is NMC wind - object
  //! will be initialized in constructor.
  NMCVecfield* NMCVec {nullptr};

  //-------------------------------------------------------------------------

  Vecfield ();
  Vecfield ( const Source_Type& SOURCE_TYPE );
  ~Vecfield ();

  //! \brief Inits Vecfield with some source
  //! \param source type
  void init ( const Source_Type& SOURCE_TYPE );

  //! \brief sets the vector field model (standard, specific
  //! interpolation model etc.: see MODE_VEC_ constants
  void
  set_model ( int model )
  {
    mode = model;
  }

  //! \brief Returns vector (3D) global coordinates at extrinsic
  //! coordinates (x,y).
  void
  get_at_xy ( const vec3d& x, vec3d* pv );

  //! \brief Returns wind in (x, y, z) representation at specified
  //! lat-lon coords in degrees
  //! !! inner approximation
  inline vec3d
  get_at_lat_lon_deg ( double lat, double lon )
  {
    return get_at_lat_lon_rad( Coordinates::deg_to_rad( lat ),
                               Coordinates::deg_to_rad( lon ) );
  }

  //! \brief Returns wind in (x, y, z) representation at specified
  //! lat-lon coords in radians
  //! !! inner approximation
  vec3d
  get_at_lat_lon_rad ( double lat, double lon );

//  //! \brief Simple assignment operator
//  Vecfield& operator= (const Vecfield& VF )
//  {
//    FIELD_SOURCE_TYPE = VF.FIELD_SOURCE_TYPE;
//    NMCVec = VF.NMCVec;
//    return *this;
//  }

private:
  // DANGER: HARDCODED CONSTANT FOR NMC GRID
  const double nmc_grid_step
    { 2.5 / 180 * M_PI };

  //! \brief longitude and latitude grid steps
  double nmc_grid_step_lo{};
  double nmc_grid_step_la{};

  //! \brief Number of longitudes
  int lon_size{0};

  //! \brief Number of latitudes
  int lat_size{0};

  int mode;

  //! \brief Implementation of filed1 from Fuselier, Edward J and
  //! Wright, Grady B article.
  void
  field1 ( const vec3d& x, vec3d* pv );

  //-------------------------------------------------------------------------

  //! \brief Normalizing longitude index
  inline int
  norm_lon_ind ( int lon_i )
  {
    return (lon_i < 0) ?
        (lon_i % lon_size) + lon_size :
        (lon_i % lon_size);

//    while ( lon_i >= lon_size )
//      lon_i -= lon_size;
//    while ( lon_i < 0 )
//      lon_i += lon_size;
//    return lon_i;
  }

  //! \brief Normalizing latitude index
  inline int
  norm_lat_ind ( int lat_i )
  {
    return (lat_i < 0) ?
        (lat_i % lat_size) + lat_size :
        (lat_i % lat_size);

//    while ( lat_i >= lat_size )
//      lat_i -= lat_size;
//    while ( lat_i < 0 )
//      lat_i += lat_size;
//    return lat_i;
  }

};

////----------------------------------------------------------------------------
////--------------------------- NMC Vec Field ----------------------------------
////----------------------------------------------------------------------------
//
//class NMCVecfield
//{
//protected:
//	friend class Sikupy;
//
//	struct GridNode	// Just for smart packing
//	{
//	public:
//		double lat;
//		double lon;
//		vec3d value;
//
//		GridNode(): lat(0), lon(0){}
//		GridNode(const double& la, const double& lo, const vec3d& val):
//			lat( la ), lon( lo ), value( val ) {}
//
//	};
//
//	std::vector< std::vector< vec3d > > grid;
//	std::map< double, size_t > lat_indexer;
//	std::map< double, size_t > lon_indexer;
//	std::map< size_t, double > lat_valuator;
//	std::map< size_t, double > lon_valuator;
//
//public:
//
//	NMCVecfield(){}
//	~NMCVecfield(){ clear(); }
//
//	vec3d* get_vec( const size_t& lat_i, const size_t& lon_i );
//	vec3d* get_vec( const double& lat, const double& lon );
//	GridNode get_node( const size_t& lat_i, const size_t& lon_i );
//	GridNode get_node( const double& lat, const double& lon );
//
//	void set_vec( const vec3d& value, const size_t& lat_i, const size_t& lon_i );
//	void set_vec( const vec3d& value, const double& lat, const double& lon );
//	void set_node( const GridNode& GN, const size_t& lat_i, const size_t& lon_i );
//
//	void init_grid( const size_t& lat_s, const size_t& lon_s );
//	void clear();
//};

#endif      /* VECFIELD_HH */
