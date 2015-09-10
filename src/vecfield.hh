/*!

  \file vecfield.hh

  \brief Vector field class. A basic class to contains air wind and
  sea streams vector fields, produce interpolation and provide values
  of the vector field in particular points.

*/

#ifndef VECFIELD_HH
#define VECFIELD_HH

#include "siku.hh"

#include <map>
#include <vector>
#include <string>
#include <stdexcept>

class Vecfield
{
 public:

  //! \brief Flag points to standard field from Fuselier paper
  static const int MODE_VEC_STD_FIELD1 {1};

  Vecfield();

  //! \brief sets the vector field model (standard, specific
  //! interpolation model etc.: see MODE_VEC_ constants
  void set_model( int model )
  { mode = model; };

  //! \brief Returns vector (3D) global coordinates at extrinsic
  //! coordinates (x,y).
  void get_at_xy( const vec3d& x,
                  vec3d* pv );

 private:

  int mode;

  //! \brief Implementation of filed1 from Fuselier, Edward J and
  //! Wright, Grady B article.
  void field1( const vec3d& x, vec3d* pv );

};

//----------------------------------------------------------------------------
//--------------------------- NMC Vec Field ----------------------------------
//----------------------------------------------------------------------------

class NMCVecfield
{
protected:
	friend class Sikupy;

	struct GridNode	// Just for smart packing
	{
	public:
		double lat;
		double lon;
		vec3d value;

		GridNode(): lat(0), lon(0){}
		GridNode(const double& la, const double& lo, const vec3d& val):
			lat( la ), lon( lo ), value( val ) {}

	};

	std::vector< std::vector< vec3d > > grid;
	std::map< double, size_t > lat_indexer;
	std::map< double, size_t > lon_indexer;
	std::map< size_t, double > lat_valuator;
	std::map< size_t, double > lon_valuator;

public:

	NMCVecfield(){}
	~NMCVecfield(){ clear(); }

	vec3d* get_vec( const size_t& lat_i, const size_t& lon_i );
	vec3d* get_vec( const double& lat, const double& lon );
	GridNode get_node( const size_t& lat_i, const size_t& lon_i );
	GridNode get_node( const double& lat, const double& lon );

	void set_vec( const vec3d& value, const size_t& lat_i, const size_t& lon_i );
	void set_vec( const vec3d& value, const double& lat, const double& lon );
	void set_node( const GridNode& GN, const size_t& lat_i, const size_t& lon_i );

	void init_grid( const size_t& lat_s, const size_t& lon_s );
	void clear();
};

////  alternative with template. BUT OLD different object structure
////--------------------------------------------------------------------------
////-------------------------- NMC Vec Field ---------------------------------
////--------------------------------------------------------------------------
//
//#include "nmc_reader.hh"
//
//class NMCVecfield : public NMC_Grid<vec3d>
//{
//public:
//	//NMC_Wind_Reader(){}
//	//~NMC_Wind_Reader(){}
//};


#endif      /* VECFIELD_HH */
