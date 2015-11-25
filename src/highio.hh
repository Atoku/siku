/*!

  \file highio.hh

  \brief Highest interface to save files with all global variables

*/

#ifndef HIGHIO_HH
#define HIGHIO_HH

#include "globals.hh"
#include "lowio.hh"

//! \brief Higher interface to read and write "global" variables from
//! and to a dump file
class Highio
{
public:
  struct Dims
  {
    size_t contact_s;
    size_t control_s;
    size_t elem_s;
    size_t vert_s;
    size_t monit_s;
    size_t grid_s;
  };

  // ---------------------------------------------------------------------
private:

  // vector of polygon vertices for saving and loading
  vector<Element::vertex> verts;

public:

  static const int STATUS_OK       { 0x0 }; //!< OK status code
  static const int STATUS_ERR_FILE { 0x1 }; //!< Error with file creation 
  
  //! \brief save the main dump file with all the information about
  //! the run, the file name to save is also a part of globals
  //! \param[in] siku all global variables
  //! \return error code
  int save ( const Globals& siku );
  
  //! \brief Load the main dump file with all the information about
  //! \param[in] siku all global variables
  //! \param[in] file_name name of file to load from
  int load ( Globals& siku, const string& file_name );

private:

  //! \brief main object for high I/O put here to avoid extra type registrations
  Lowio lowio;

  //! \brief fill vertices vector with polygon vertices` coordinates for saving
  //! !!IMPORTANT: works with local coordinates
  //! \param[in] siku global variables
  void presave_verts( const Globals& siku );

//// YET UNTESTED
  void save_info( const Globals& siku );
  void save_planet( const Globals& siku );
  void save_materials( const Globals& siku );
  void save_vecfield( const Globals& siku );
  void save_diagnostics( const Globals& siku );
  void save_condet( const Globals& siku );

  // ----------------------- support methods --------------------------------

  //! \brief Save single Diagbase with inner array
  int save_diag ( const string& location, void* pdiag );

  //! \brief Save single material with all inner arrays
  int save_material ( const string& location, void* pmat );

  //! \brief Save single mesh with inner array
  int save_mesh ( const string& location, void* pmesh );

  //! \brief Save NMC class grid
  int save_nmc( const string& location, void* pnmc);

  //! \brief load dimensions
  void load_dims( Highio::Dims& dims );

};

#endif      /* HIGHIO_HH */
