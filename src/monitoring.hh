/*!

  \file monitoring.hh

  \brief Monitoring and diagnosting. It includes monitoring of
  "monitored" elements and diagnosting function.

*/

#ifndef MONITORING_HH
#define MONITORING_HH

#include "globals.hh"
#include "sikupy.hh"

//! \brief Call the monitor functions on the elements that have
//! appropriate monitor functions
void monitoring( Globals& siku, Sikupy& sikupy );

//! \brief Create all the arrays and call all the functions for
//! diagnosting
void diagnosting( Globals& siku, Sikupy& sikupy );

#endif      /* MONITORING_HH */
