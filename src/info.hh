/*!

  \file info.hh

  \brief General information about the model (usually restored from
  siku.info dictionary of configuration file)

*/

#ifndef INFO_HH
#define INFO_HH

#include <string>
using namespace std;

//! \brief General information about the model and current run
struct Info
{
  string name;                  //!< program name
  string brief;                 //!< description
  string version;               //!< version string
  string date;                  //!< date when program created
  string rundate;               //!< date when program runs
};

#endif      /* INFO_HH */
