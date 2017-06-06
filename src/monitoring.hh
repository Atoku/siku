/*
 * Siku: Discrete element method sea-ice model: description.hh
 *
 * Copyright (C) UAF
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */

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
