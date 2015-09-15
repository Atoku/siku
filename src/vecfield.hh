/*!

  \file vecfield.hh

  \brief Vector field class. A basic class that contains air wind and
  sea streams vector fields, produces interpolation and provides
  values of the vector field at particular points.

*/

#ifndef VECFIELD_HH
#define VECFIELD_HH

//#include <map>
//#include <vector>
//#include <string>
//#include <stdexcept>

#include "siku.hh"
#include "globals.hh"
#include "nmc_reader.hh"

#include <cmath>

enum : unsigned int
{
  FIELD_NONE = 0,
  FIELD_NMC = 1
};

class Vecfield
{
 public:
  //! \brief Flag for vecfield source type
  unsigned int FIELD_SOURCE_TYPE {1};

  //! \brief Flag points to standard field from Fuselier paper
  static const int MODE_VEC_STD_FIELD1 {1};

  NMCVecfield* NMCWind;

  Vecfield();
  Vecfield(const unsigned int& SOURCE_TYPE);
  ~Vecfield();

  //! \brief sets the vector field model (standard, specific
  //! interpolation model etc.: see MODE_VEC_ constants
  void set_model( int model )
  { mode = model; }

  //! \brief Returns vector (3D) global coordinates at extrinsic
  //! coordinates (x,y).
  void get_at_xy( const vec3d& x,
                  vec3d* pv );

  int load_wind( Globals& siku );
  vec3d get_at_lat_lon_rad( double lat, double lon) ;

 private:

  // DANGER: HARDCODED CONSTANT FOR NMC
  const double nmc_grid_step{ 2.5 / 180 * M_PI };

  int mode;

  //! \brief Implementation of filed1 from Fuselier, Edward J and
  //! Wright, Grady B article.
  void field1( const vec3d& x, vec3d* pv );

  //-------------------------------------------------------------------------

  inline size_t norm_lon_ind( size_t lon_i )
  {
    size_t size = NMCWind->get_lon_size();
    while( lon_i > size )
      lon_i -= size;
    while( lon_i < size )
      lon_i += size;
    return lon_i;
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
