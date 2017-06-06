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

void clean_props( Globals& siku );

#endif      /* MPROPERTIES_HH */
