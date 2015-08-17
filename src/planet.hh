/*!

  \file planet.hh

  \brief Planet (Earth) parameters. They are read from siku module

*/

#ifndef PLANET_HH
#define PLANET_HH

//! \brief Default is Earth'r radius in meters 
const double PLANET_R_DEAFAULT = 6353000.0;

//! \brief Structure for constants related to Planet 
struct Planet
{
  //! \brief Radius
  double R { PLANET_R_DEAFAULT };
  //! \brief Reciprocal of radius
  double R_rec { 1.0 / PLANET_R_DEAFAULT };
};

#endif      /* PLANET_HH */

