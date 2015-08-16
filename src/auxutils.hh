/*!

  \file auxutils.hh

  \brief Auxiliary utilities for all differnt purposes

*/

#ifndef AUXUTILS_HH
#define AUXUTILS_HH

#include <string>
using namespace std;

namespace auxutils
{
  //! \brief creates a string without file extension
  string remove_file_extension( const string& filename );
}

#endif      /* AUXUTILS_HH */
