/*!

  \file mproperties.hh

  \brief Mechanical properties update. As mass and moment of inertia
  of an element may change with time, sometimes mass and moment of
  inertia values need to be updated. This function traverse element
  list and do exactly this update.

*/

#ifndef MPROPERTIES_HH
#define MPROPERTIES_HH

#include "globals.hh"

void mproperties( Globals& siku );

#endif      /* MPROPERTIES_HH */
