/*!

 \file dynamics.hh

 \brief Dynamics integrator.

 */

#ifndef DYNAMICS_HH
#define DYNAMICS_HH

#include "globals.hh"

//! \brief update of velocities and angular velocity depending on
//! forces
void
dynamics ( Globals& siku, const double dt );

#endif      /* DYNAMICS_HH */
