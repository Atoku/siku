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
  vector<Element::vertex> verts;

  static const int STATUS_OK       { 0x0 }; //!< OK status code
  static const int STATUS_ERR_FILE { 0x1 }; //!< Error with file creation 
  
  //! \brief save the main dump file with all the information about
  //! the run, the file name to save is also a part of globals
  //! \param[in] siku all global variables
  //! \return error code
  int save ( const Globals& siku );
  
private:

  //! \brief main object for high I/O put here to avoid extra type registrations
  Lowio lowio;

  //! \brief fill vertices vector with polygon vertices` coordinates for saving
  //! !!IMPORTANT: works with local coordinates
  //! \param[in] siku global variables
  void fill_verts( const Globals& siku );
};

#endif      /* HIGHIO_HH */
