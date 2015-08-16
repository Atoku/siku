/*!

  \file auxutils.cc

  \brief Implementation of auxutils namespace, miscelleneous auxiliary
  utilities

*/

#include "auxutils.hh"

string auxutils::remove_file_extension( const string& filename )
{
  size_t lastdot = filename.find_last_of( "." );
  if ( lastdot == std::string::npos )
    return filename;
  
  return filename.substr( 0, lastdot );
}

