/*!

  \file forces_mass.hh

  \brief Mass forces computaton and assignement that include drivers
  forces from wind and currents, Coriolis forces, Gravitational
  buyoncy etc.

*/

#ifndef FORCES_MASS_HH
#define FORCES_MASS_HH

#include "globals.hh"

//! \brief Update the forces by mass forces
void forces_mass( Globals& siku );

#endif      /* FORCES_MASS_HH */
