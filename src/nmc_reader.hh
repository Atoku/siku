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

  friend class Sikupy;

public:
  //! \brief inner structure (POD) for simple returns
  struct GridNode // Just for smart packing
  {
  public:
    double lat;
    double lon;
    vec3d value;

    GridNode () : lat { 0 }, lon { 0 }  {}
    GridNode ( const double& la, const double& lo, const vec3d& val ) :
        lat { la }, lon { lo }, value ( val )  {}
  };

protected:
public://////////////////////////////////////////////////////////////////////////////////////////////
  //! \brief the wind velocity value itself (storaged as vec3d)
  std::vector < std::vector < vec3d > > grid;

  //! \brief maps for converting geographical lat-lon values into grid
  //! indexes and vice versa
  std::map < double, size_t > lat_indexer;
  std::map < double, size_t > lon_indexer;
  std::map < size_t, double > lat_valuator;
  std::map < size_t, double > lon_valuator;

  //double lat_step{ 0. };
  //double lon_step{ 0. };
public:

  long time_step { 0 };

  NMCVecfield ()
  {
    grid = std::vector < std::vector < vec3d > >(0);
  }
  ~NMCVecfield ()
  {
    clear ();
  }

  //! \brief Returns the value by specified indexes
  vec3d
  get_vec ( const int& lat_i, const int& lon_i );

  //! \brief Returns the value by specified coordinates
  //! (may couse precision errors!!)
  vec3d
  get_vec ( const double& lat, const double& lon );

  //! \brief Returns entire node by specified indexes
  GridNode
  get_node ( const size_t& lat_i, const size_t& lon_i );

  //! \brief Returns entire node by specified coordinates
  //! (may couse precision errors!!)
  GridNode
  get_node ( const double& lat, const double& lon );

  //inline double get_lat_step(){ return lat_step; }
  //inline double get_lon_step(){ return lon_step; }

  //! \brief Returns amount of latitude indexes
  inline size_t  get_lat_size ()  { return grid.size (); }

  //! \brief Returns amount of longitude indexes
  inline size_t  get_lon_size ()  { return grid[0].size (); }

  //! \brief Returns minimal latitude value
  inline double get_min_lat () { return lat_indexer.begin()->first; }

  //! \brief Returns maximal latitude value
  inline double get_max_lat () { return lat_indexer.rbegin()->first; }

  //! \brief Returns minimal longitude value
  inline double get_min_lon () { return lon_indexer.begin()->first; }

  //! \brief Returns maximal longitude value
  inline double get_max_lon () { return lon_indexer.rbegin()->first; }

  //! \brief Sets the value at specified indexes
  void
  set_vec ( const vec3d& value, const size_t& lat_i, const size_t& lon_i );

  //! \brief Sets the value at specified coordinates
  void
  set_vec ( const vec3d& value, const double& lat, const double& lon );

  //! \brief Sets entire node at specified indexes
  void
  set_node ( const GridNode& GN, const size_t& lat_i, const size_t& lon_i );

  //! \brief Constructs the grid of required size
  void
  init_grid ( const size_t& lat_s, const size_t& lon_s );

  //! \brief Clears the grid
  void
  clear ();
};

#endif /* NMC_READER_HH */
