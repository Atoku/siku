/*!

 \file nmc_reader.hh

 \brief Several classes for generic and specific reading of
 NCEP/NCAR Reanalysis grids.
 Base class provides datagrid template and interaction with python nmc
 modules.
 Derived classes provide specific value reading.

 */

#ifndef NMC_READER_HH
#define NMC_READER_HH

//#include <Python.h>
//#undef tolower	//	just as in all other python-dependent modules

#include <map>
#include <vector>
#include <string>
#include <stdexcept>

#include "siku.hh"

//----------------------------------------------------------------------------
//--------------------------- NMC Vec Field ----------------------------------
//----------------------------------------------------------------------------

class NMCVecfield
{
protected:
  friend class Sikupy;

  struct GridNode // Just for smart packing
  {
  public:
    double lat;
    double lon;
    vec3d value;

    GridNode () :
        lat ( 0 ), lon ( 0 )
    {
    }
    GridNode ( const double& la, const double& lo, const vec3d& val ) :
        lat ( la ), lon ( lo ), value ( val )
    {
    }

  };

  std::vector < std::vector < vec3d > > grid;
  std::map < double, size_t > lat_indexer;
  std::map < double, size_t > lon_indexer;
  std::map < size_t, double > lat_valuator;
  std::map < size_t, double > lon_valuator;

  //double lat_step{ 0. };
  //double lon_step{ 0. };
public:

  NMCVecfield ()
  {
  }
  ~NMCVecfield ()
  {
    clear ();
  }

  vec3d
  get_vec ( const size_t& lat_i, const size_t& lon_i );
  vec3d
  get_vec ( const double& lat, const double& lon );
  GridNode
  get_node ( const size_t& lat_i, const size_t& lon_i );
  GridNode
  get_node ( const double& lat, const double& lon );

  //inline double get_lat_step(){ return lat_step; }
  //inline double get_lon_step(){ return lon_step; }
  inline size_t
  get_lat_size ()
  {
    return grid.size ();
  }
  inline size_t
  get_lon_size ()
  {
    return grid[0].size ();
  }

  void
  set_vec ( const vec3d& value, const size_t& lat_i, const size_t& lon_i );
  void
  set_vec ( const vec3d& value, const double& lat, const double& lon );
  void
  set_node ( const GridNode& GN, const size_t& lat_i, const size_t& lon_i );

  void
  init_grid ( const size_t& lat_s, const size_t& lon_s );
  void
  clear ();
};

////REMOVE AFTER AUXUTILS INCLUDE!!!!
//std::string remove_file_extension( const std::string& filename );
////AAAAA! REMOVE IT -^
//
//#include "iostream"
////#include "siku.hh"
//
//template<class C>
//class NMC_Grid
//{
////public:
//protected:
//
//	template<class T>
//	class GridNode
//	{
//	public:
//		double lat;
//		double lon;
//		T value;
//
//		GridNode(): lat(0), lon(0){}
//		GridNode(const double& la, const double& lo, const T& val):
//			lat( la ), lon( lo ), value( val ) {}
//
//	};
//
//	std::vector< std::vector< GridNode< C > > > grid;
//	std::map< double, size_t > lat_indexer;
//	std::map< double, size_t > lon_indexer;
//
//	PyObject *pModule {nullptr};            //!< NMC python module (new)
//	PyObject *pNMCVar {nullptr};            //!< python NMCVar (borrowed)
//	PyObject *pTemp {nullptr};              //!< temporal variable (borrowed)
//
//public:
//
//	NMC_Grid(){}
//	~NMC_Grid();
//
//	C* get_value( const size_t& lat_i, const size_t& lon_i );
//	C* get_value_by_lat_lon( const double& lat, const double& lon );
//	GridNode< C >* get_node( const size_t& lat_i, const size_t& lon_i );
//	GridNode< C >* get_node_by_lat_lon( const double& lat, const double& lon );
//
//	void set_value( const C& value, const size_t& lat_i, const size_t& lon_i );
//	void set_value( const C& value, const double& lat, const double& lon );
//	void set_node( const GridNode<C>& GN, const size_t& lat_i, const size_t& lon_i );
//
//	void init_grid( const size_t& lat_s, const size_t& lon_s );
//	void clear();
//};
//
//template<class C>
//NMC_Grid< C >::~NMC_Grid()
//{
//	clear();
//}
//
//template<class C>
//C* NMC_Grid< C >::get_value( const size_t& lat_i, const size_t& lon_i )
//{
//	try
//	{
//		return &grid.at( lat_i ).at( lon_i ).value;
//	}
//	catch(const std::out_of_range& oor)
//	{
//		return nullptr;
//	}
//}
//template<class C>
//C* NMC_Grid< C >::get_value_by_lat_lon( const double& lat, const double& lon )
//{
//	try
//	{
//		return &grid.at( lat_indexer.at( lat ) ).at( lon_indexer.at( lon ) ).value;
//	}
//	catch(const std::out_of_range& oor)
//	{
//		return nullptr;
//	}
//}
//template<class C>
//NMC_Grid< C >::GridNode< C >* NMC_Grid< C >::get_node( const size_t& lat_i, const size_t& lon_i )
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
//template<class C>
//NMC_Grid< C >::GridNode< C >* NMC_Grid< C >::get_node_by_lat_lon( const double& lat, const double& lon )
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
//
//template<class C>
//void NMC_Grid< C >::set_value( const C& value, const size_t& lat_i, const size_t& lon_i )
//{
//	try
//	{
//		grid.at( lat_i ).at( lon_i ).value = value;
//	}
//	catch(const std::out_of_range& oor)
//	{
//		//error: "illegal index"
//		throw(oor);
//	}
//}
//template<class C>
//void NMC_Grid< C >::set_value( const C& value, const double& lat, const double& lon )
//{
//	try
//	{
//		grid.at( lat_indexer.at( lat ) ).at( lon_indexer.at( lon ) ).value = value;
//	}
//	catch(const std::out_of_range& oor)
//	{
//		//error: "bad argument: no such coordinates"
//		throw(oor);
//	}
//}
//template<class C>
//void NMC_Grid< C >::set_node( const GridNode<C>& GN, const size_t& lat_i, const size_t& lon_i )
//{
//	try
//	{
//		grid.at( lat_i ).at( lon_i ) = GN;
//		lat_indexer[ GN.lat ] = lat_i;
//		lon_indexer[ GN.lon ] = lon_i;
//	}
//	catch(const std::out_of_range& oor)
//	{
//		//error: "illegal index"
//		throw(oor);
//	}
//}
//
//template<class C>
//void NMC_Grid< C >::init_grid( const size_t& lat_s, const size_t& lon_s )
//{
//	clear();
//	grid.resize( lat_s, std::vector< GridNode< C > > ( lon_s ) );
//}
//template<class C>
//void NMC_Grid< C >::clear()
//{
//	for( size_t i=0; i < grid.size(); ++i)
//		grid[i].clear();
//	grid.clear();
//	lat_indexer.clear();
//	lon_indexer.clear();
//}
//
//
//
////----------------------------------------------------------------------------
////------------------------------- NMC_Reader ---------------------------------
////----------------------------------------------------------------------------
//
//struct UVWind
//{
//	double uwind;
//	double vwind;
//	UVWind(){}
//	UVWind( const double& u, const double& v ) : uwind(u), vwind(v) {}
//};
//
//class NMC_Wind_Reader : public NMC_Grid<UVWind>
//{
//public:
//	NMC_Wind_Reader(){}
//	NMC_Wind_Reader( std::string filename) { load( filename); }
//	//~NMC_Wind_Reader(){}
//
//	void load( std::string filenam );
//};

#endif /* NMC_READER_HH */
